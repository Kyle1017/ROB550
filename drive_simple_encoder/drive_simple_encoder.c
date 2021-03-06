/**
 * @file rc_project_template.c
 *
 * This is meant to be a skeleton program for Robot Control projects. Change
 * this description and file name before modifying for your own purpose.
 */

#include <stdio.h>
#include <robotcontrol.h> // includes ALL Robot Control subsystems

// function declarations
void on_pause_press();
void on_pause_release();


/**
 * This template contains these critical components
 * - ensure no existing instances are running and make new PID file
 * - start the signal handler
 * - initialize subsystems you wish to use
 * - while loop that checks for EXITING condition
 * - cleanup subsystems at the end
 *
 * @return     0 during normal operation, -1 on error
 */
int main()
{
	// make sure another instance isn't running
	// if return value is -3 then a background process is running with
	// higher privaledges and we couldn't kill it, in which case we should
	// not continue or there may be hardware conflicts. If it returned -4
	// then there was an invalid argument that needs to be fixed.
	if(rc_kill_existing_process(2.0)<-2) return -1;

	// start signal handler so we can exit cleanly
	if(rc_enable_signal_handler()==-1){
		fprintf(stderr,"ERROR: failed to start signal handler\n");
		return -1;
	}

	// initialize pause button
	if(rc_button_init(RC_BTN_PIN_PAUSE, RC_BTN_POLARITY_NORM_HIGH,
						RC_BTN_DEBOUNCE_DEFAULT_US)){
		fprintf(stderr,"ERROR: failed to initialize pause button\n");
		return -1;
	}

	// Assign functions to be called when button events occur
	rc_button_set_callbacks(RC_BTN_PIN_PAUSE,on_pause_press,on_pause_release);

	// make PID file to indicate your project is running
	// due to the check made on the call to rc_kill_existing_process() above
	// we can be fairly confident there is no PID file already and we can
	// make our own safely.
	rc_make_pid_file();


	printf("\nsimple square drive\n");
	
	
	// Keep looping until state changes to EXITING
	//rc_set_state(RUNNING);
/* 	while(rc_get_state()!=EXITING){
		// do things based on the state
		if(rc_get_state()==RUNNING){
			rc_led_set(RC_LED_GREEN, 1);
			rc_led_set(RC_LED_RED, 0);
			rc_encoder_eqep_init();
			enc_reading1 = rc_encoder_eqep_read(1);
			printf("%d",enc_reading1);
			rc_usleep(0.5E6); 
		}
		else{
			rc_led_set(RC_LED_GREEN, 0);
			rc_led_set(RC_LED_RED, 1);
			rc_encoder_eqep_cleanup();
		}
		// always sleep at some point
		rc_usleep(100000);
	} */
	//printf("%d",enc_reading1);
	int enc_1m;
	enc_1m = 6200;
	int enc_90deg;
	enc_90deg = 800;
	int i;
	//const int samples = 100; // check for release 100 times in this period
	const int test_run_time = 3; 
	
	rc_motor_init();
	rc_encoder_eqep_init();
	int enc_reading1;
	enc_reading1 = rc_encoder_eqep_read(1);
	int enc_reading2;
	enc_reading2 = rc_encoder_eqep_read(2);
	// now keep checking to see if the button is still held down
	for(i=0;i<test_run_time;i++){
		while(enc_reading1 < enc_1m && -enc_reading2 < enc_1m){
			rc_motor_set (1, -0.245);
			rc_motor_set (2, 0.25);
			printf("%d \n",enc_reading1);
			printf("%d \n",enc_reading2);
			enc_reading1 = rc_encoder_eqep_read(1);
			enc_reading2 = rc_encoder_eqep_read(2);
		}
		rc_encoder_eqep_cleanup();
		rc_encoder_eqep_init();
		enc_reading1 = rc_encoder_eqep_read(1);
		enc_reading2 = rc_encoder_eqep_read(2);
		while(enc_reading1 < enc_90deg && enc_reading2 < enc_90deg){
			rc_motor_set (1, -0.240);
			rc_motor_set (2, -0.245);
			enc_reading1 = rc_encoder_eqep_read(1);
			enc_reading2 = rc_encoder_eqep_read(2);
		}
	} 
	//rc_usleep(0.05E6);
	rc_motor_set (1, -0.245);
	rc_motor_set (2, 0.25);
    rc_usleep(4.2E6); 
	//rc_usleep(0.6E6); 
	rc_motor_set (1, 0.0);
	rc_motor_set (2, 0.0);
	rc_encoder_eqep_cleanup();
    rc_motor_cleanup(); 
	// turn off LEDs and close file descriptors
	rc_led_set(RC_LED_GREEN, 0);
	rc_led_set(RC_LED_RED, 0);
	rc_led_cleanup();
	rc_button_cleanup();	// stop button handlers
	rc_remove_pid_file();	// remove pid file LAST
	return 0;
}


/**
 * Make the Pause button toggle between paused and running states.
 */
void on_pause_release()
{
	if(rc_get_state()==RUNNING)	rc_set_state(PAUSED);
	else if(rc_get_state()==PAUSED)	rc_set_state(RUNNING);
	return;
}

/**
* If the user holds the pause button for 2 seconds, set state to EXITING which
* triggers the rest of the program to exit cleanly.
**/
void on_pause_press()
{
	int i;
	const int samples = 100; // check for release 100 times in this period
	const int us_wait = 2000000; // 2 seconds

	// now keep checking to see if the button is still held down
	for(i=0;i<samples;i++){
		rc_usleep(us_wait/samples);
		if(rc_button_get_state(RC_BTN_PIN_PAUSE)==RC_BTN_STATE_RELEASED) return;
	}
	printf("long press detected, shutting down\n");
	rc_set_state(EXITING);
	return;
}
