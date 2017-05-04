/** \file shapemotion.c
 *  \brief This is a simple shape motion demo.
 *  This demo creates two layers containing shapes.
 *  One layer contains a rectangle and the other a circle.
 *  While the CPU is running the green LED is on, and
 *  when the screen does not need to be redrawn the CPU
 *  is turned off along with the green LED.
 */  
#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>

#define GREEN_LED BIT6

//keeps scores going
int score1 = 0;
int score2 = 0;

AbRect rect10 = {abRectGetBounds, abRectCheck, {2 ,15}};
/**< 10x10 rectangle */

AbRectOutline fieldOutline = {	/* playing field */
		abRectOutlineGetBounds, abRectOutlineCheck,   
		{screenWidth/2 - 10, screenHeight/2 - 10}
};

Layer layer3 = {		/**< Layer with an yellow circle */
		(AbShape *)&circle4,
		{(screenWidth/2)+10, (screenHeight/2)+5}, /**< bit below & right of center */
		{0,0}, {0,0},				    /* last & next pos */
		COLOR_YELLOW
};


Layer fieldLayer = {		/* playing field as a layer */
		(AbShape *) &fieldOutline,
		{screenWidth/2, screenHeight/2},/**< center */
		{0,0}, {0,0},				    /* last & next pos */
		COLOR_GREEN,
		&layer3
};

Layer layer1 = {		/**< Layer with a red palette */
		(AbShape *)&rect10,
		{14, 30}, /**< center */
		{0,0}, {0,0},				    /* last & next pos */
		COLOR_RED,
		&fieldLayer,
};

Layer layer0 = {		/**< Layer with a blue palette */
		(AbShape *)&rect10,
		{screenWidth-14, screenHeight-30}, /**< on the other side of the screen */
		{0,0}, {0,0},				    /* last & next pos */
		COLOR_BLUE,
		&layer1,
};

/** Moving Layer
 *  Linked list of layer references
 *  Velocity represents one iteration of change (direction & magnitude)
 */
typedef struct MovLayer_s {
	Layer *layer;
	Vec2 velocity;
	struct MovLayer_s *next;
} MovLayer;

/* initial value of {0,0} will be overwritten */
MovLayer ml3 = { &layer3, {3,3}, 0 }; /**< not all layers move */
MovLayer ml1 = { &layer1, {0,0}, &ml3 }; 
MovLayer ml0 = { &layer0, {0,0}, &ml1 }; 
/************************************************/
void movLayerDraw(MovLayer *movLayers, Layer *layers)
{
	int row, col;
	MovLayer *movLayer;

	and_sr(~8);/**< disable interrupts (GIE off) */
	for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
		Layer *l = movLayer->layer;
		l->posLast = l->pos;
		l->pos = l->posNext;
	}
	or_sr(8);			/**< disable interrupts (GIE on) */


	for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
		Region bounds;
		layerGetBounds(movLayer->layer, &bounds);
		lcd_setArea(bounds.topLeft.axes[0], bounds.topLeft.axes[1], bounds.botRight.axes[0], bounds.botRight.axes[1]);
		for (row = bounds.topLeft.axes[1]; row <= bounds.botRight.axes[1]; row++) {
			for (col = bounds.topLeft.axes[0]; col <= bounds.botRight.axes[0]; col++) {
				Vec2 pixelPos = {col, row};
				u_int color = bgColor;
				Layer *probeLayer;
				for (probeLayer = layers; probeLayer; 
						probeLayer = probeLayer->next) { /* probe all layers, in order */
					if (abShapeCheck(probeLayer->abShape, &probeLayer->pos, &pixelPos)) {
						color = probeLayer->color;
						break; 
					} /* if probe check */
				} // for checking all layers at col, row
				lcd_writeColor(color); 
			} // for col
		} // for row
	} // for moving layer being updated
}	  



Region fence = {{10,30}, {SHORT_EDGE_PIXELS-10, LONG_EDGE_PIXELS-10}}; /**< Create a fence region */

/** Advances a moving shape within a fence
 *  
 *  \param ml The moving shape to be advanced
 *  \param fence The region which will serve as a boundary for ml
 */
/************************************************/
void mlAdvance(MovLayer *ml, Region *fence, Region *p1, Region *p2)
{
	Vec2 newPos;
	u_char axis;
	Region shapeBoundary;
	for (; ml; ml = ml->next) {
		vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
		abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
		for(axis = 0; axis < 2; axis++){
			if ((shapeBoundary.topLeft.axes[axis] < fence -> topLeft.axes[axis]) ||
					(shapeBoundary.botRight.axes[axis] > fence -> botRight.axes[axis])){
				int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
				newPos.axes[axis] += (2*velocity);
			}/**< if outside of fence */
		}

		if(shapeBoundary.topLeft.axes[0] < fence->topLeft.axes[0]){
			buzzer_init(3500);
			score2++;
		}else if(shapeBoundary.botRight.axes[0] > fence -> botRight.axes[0]){
			buzzer_init(3500);
			score1++;
		}else{
			buzzer_init(0);
		}

		//check for Player 1
		if ((shapeBoundary.topLeft.axes[0] < p1->botRight.axes[0]) && 
				(shapeBoundary.topLeft.axes[0] > p1->topLeft.axes[0])) {
			if((shapeBoundary.botRight.axes[1] >= p1->topLeft.axes[1]) && 
					(shapeBoundary.topLeft.axes[1] <= p1->botRight.axes[1])) {
				ml->velocity.axes[0] = -ml->velocity.axes[0];
			}
		}
		//check for Player 2
		if ((shapeBoundary.botRight.axes[0] > p2->topLeft.axes[0]) 
				&& (shapeBoundary.botRight.axes[0] < p2->botRight.axes[0])) {
			if((shapeBoundary.botRight.axes[1] >= p2->topLeft.axes[1]) 
					&& (shapeBoundary.topLeft.axes[1] <= p2->botRight.axes[1])) {
				ml->velocity.axes[0] = -ml->velocity.axes[0];
			}
		}

		ml->layer->posNext = newPos;
	} /**< for ml */
}

/*
 * tuns on buzzer
 */
/************************************************/
void buzzer_init(short cycles){
	timerAUpmode();
	P2SEL2 &= ~(BIT6 | BIT7);
	P2SEL &= ~BIT7;
	P2SEL |= BIT6;
	P2DIR = BIT6;

	CCR0 = cycles;
	CCR1 = cycles >> 1;
}

u_int bgColor = COLOR_BLACK;     /**< The background color */
int redrawScreen = 1;           /**< Boolean for whether screen needs to be redrawn */

Region fieldFence;		/**< fence around playing field  */
Region plyr1;       //creates region for the red palette
Region plyr2;       //creates region for the blue palette

/** Initializes everything, enables interrupts and green LED, 
 *  and handles the rendering for the screen
 */
/************************************************/
void main()
{
	P1DIR |= GREEN_LED;		/**< Green led on when CPU on */		
	P1OUT |= GREEN_LED;

	configureClocks();
	lcd_init();
	shapeInit();
	p2sw_init(15);

	shapeInit();

	layerInit(&layer0);
	layerDraw(&layer0);
	layerGetBounds(&fieldLayer, &fieldFence);
	layerGetBounds(&layer1, &plyr1);          //creates bounds for red palette
	layerGetBounds(&layer0, &plyr2);          //creates bounds for blue palette

	enableWDTInterrupts();      /**< enable periodic interrupt */
	or_sr(0x8);	              /**< GIE (enable interrupts) */


	for(;;) { 
		while (!redrawScreen) { /**< Pause CPU if screen doesn't need updating */
			P1OUT &= ~GREEN_LED;    /**< Green led off witHo CPU */
			or_sr(0x10);	      /**< CPU OFF */
		}
		P1OUT ^= GREEN_LED;       /**< Green led on when CPU on */
		redrawScreen = 0;
		movLayerDraw(&ml0, &layer0);
	}
}

/** Watchdog timer interrupt handler. 15 interrupts/sec */
/************************************************/
void wdt_c_handler()
{
	static short count = 0;
	P1OUT |= GREEN_LED;		      /**< Green LED on when cpu on */
	count ++;
	u_int switches = p2sw_read(), i;
	char str[5];
	if (count == 15) {
		layerGetBounds(&layer1, &plyr1);
		layerGetBounds(&layer0, &plyr2);
		mlAdvance(&ml0, &fieldFence, &plyr1, &plyr2);
		for (i = 0; i < 4; i++)
			str[i] = (switches & (1<<i)) ? 0: 1;
		//moves palettes up/down depending on the button
		if(str[0]) {//button 1 up for red palette
			ml1.velocity.axes[1] = -5;
			ml1.velocity.axes[0] = 0;
		}else{
			ml1.velocity.axes[1] = 0;
			ml1.velocity.axes[0] = 0;
		}
		if(str[1]) {//button 2 down for red palette
			ml1.velocity.axes[1] = 5;
			ml1.velocity.axes[0] = 0;
		}
		if(str[2]){//button 3 up for blue palette
			ml0.velocity.axes[1] = -5;
			ml0.velocity.axes[0] = 0;
		}else{
			ml0.velocity.axes[1] = 0;
			ml0.velocity.axes[0] = 0;
		}
		if(str[3]){//button 4 down for red palette
			ml0.velocity.axes[1] = 5;
			ml0.velocity.axes[0] = 0;
		}
		if(score1 > score2){
			drawString5x7(11, screenHeight - 8, "RED IS WINNING", COLOR_RED, COLOR_BLACK);//if player 1 is winning
		}else if(score2 > score1){
			drawString5x7(11, screenHeight - 8, "BLUE IS WINNING", COLOR_BLUE, COLOR_BLACK);
		}else{
			drawString5x7(11, screenHeight - 8, "Game Is Tied!!", COLOR_PURPLE, COLOR_BLACK);
		}

		if (p2sw_read())
			redrawScreen = 1;
		count = 0;
	}
	P1OUT &= ~GREEN_LED;		    /**< Green LED off when cpu off */
}