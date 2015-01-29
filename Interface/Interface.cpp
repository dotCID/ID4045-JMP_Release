/* 	A gui client for the http://www.linuxhowtos.org/C_C++/socket.htm server client 
	for the purpose of JMP. GUI was created with help of YouTube user "thecplusplusguy"'s videos.
	@author Marien Wolthuis */
	
#include <gtk/gtk.h>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <math.h>
#include <unistd.h>
#include <clocale>

#define cAno 6
#define cASno 5	
#define cSno 2

GtkWidget *cdetIn[3], *window, *progressbar; // holds connection details
bool done = false;
char posReturn[256];
bool positionCheck = false;  // this polls the server for current position

const char* cdet[3][2] = {{"localhost", "Server address"}, // default to local use
						  {"4547", "Server port"}};

		  
const char* commArd[cAno][2] = {{"blink\n", "Blink LED 13"},
								{"on\n", "Turn on LED 13"},
								{"off\n", "Turn off LED 13"},
								{"extend\n", "Extend the arm"},
								{"retract\n", "Retract the arm"},
								{"home\n", "Retract and home"}};
		 
const char* commArdS[cASno][2] ={{"aExt", "Arm extension"},
							 	 {"aRot", "Arm rotation"},
							 	 {"hPos", "Horizontal Position"},
							 	 {"vPos", "Vertical Position"},
								 {"getPos\n", "Get current position"}};
						 
const char* commSer[cSno][2] = {{"echo\n", "Echo this message"},
							 	{"aAddr\n", "Get Arduino address"}};	


/*
	Provides a dialog with information about errors that have occurred during runtime
	@return void
	@param *msg the error message as provided by the system
*/								
void error(const char *msg)
{
    perror(msg);
    
    GtkWidget *dialog, *label, *spacer;
	dialog = gtk_dialog_new_with_buttons("Error", GTK_WINDOW(window), GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
	label = gtk_label_new(msg);
	spacer = gtk_label_new("");
	gtk_widget_set_size_request(spacer, 20, 10);

	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),spacer, false, 0, 0);
	
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),label, false, 0, 0);
	
	label = gtk_label_new("Please check your settings.");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),label, false, 0, 0);
	
	gtk_widget_set_size_request(dialog, 250, 125);
	gtk_widget_show_all(dialog);
	
	gtk_dialog_run(GTK_DIALOG(dialog)); // contains the response from dialog
	
	// widget needs to be destroyed manually
	gtk_widget_destroy(dialog);
	
    //exit(0);
}

/*
	Initialises a TCP connection to the server and port set in the "settings" tab or from default values
	@return int A status code for debugging
	@param message The message to be sent to the server
*/
int TCPMessage(const char* message)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    
    portno = atoi(cdet[1][0]);
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
        return -1;
    }
    
    server = gethostbyname(cdet[0][0]);
    
    if (server == NULL){
        error("ERROR, no such host\n");
        return -2;
    }
        

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    	return -3;
    }
        
    n = write(sockfd,message,strlen(message));
    if (n < 0) {
         error("ERROR writing to socket");
         return -4;
    }
         
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) {
         error("ERROR reading from socket");
         return -5;
    }
    
	if(positionCheck){
		if(strcmp(message, commArdS[4][0])==0 || strcmp(message, commSer[0][0])==0 || strcmp(message, commSer[1][0])==0)
    	memcpy(&posReturn, &buffer, sizeof(buffer));
		printf("%s\n",posReturn);
    }
    
    close(sockfd);
    return 0;
}				 

/*
	Callback function for when a settings button has been pressed
	@return void
	@param button The widget from which the callback originated
	@param data a pointer to the entry field that belongs to the button
*/
static void setting_responseBtn(GtkWidget *button, gpointer data)
{
	const char *label = gtk_button_get_label(GTK_BUTTON(button));
	for(int i=0;i<2;i++){
		if(strcmp(label, cdet[i][1]) ==0){
			cdet[i][0] = gtk_entry_get_text(GTK_ENTRY(data));
			g_print("%s%s%s%s\n", "Set ", cdet[i][0], " as ", cdet[i][1]);
		}
	}
	//g_print("%s\n", "Setting response completed");
}

/*
	Callback function for when a settings field was activated by pressing "Enter"
	@return void
	@param entry The entry field widget from which the callback originated
	@param data The button associated with the entry field
*/
static void setting_responseEnt(GtkWidget *entry, gpointer data){ // a bit hairy but oh well
	const char *label = gtk_button_get_label(GTK_BUTTON(data));
	for(int i=0;i<2;i++){
		if(strcmp(label, cdet[i][1]) ==0){
			cdet[i][0] = gtk_entry_get_text(GTK_ENTRY(entry));
			g_print("%s%s%s%s\n", "Set ", cdet[i][0], " as ", cdet[i][1]);
		}		
	}
}

/*
	Callback function for responding to the test command buttons on the Command tab
	@return void
	@param button The button from which the callback originated
	@param data A null pointer (unused by this function but required by definition)
*/
static void command_response(GtkWidget *button, gpointer data){
	const char *label = gtk_button_get_label(GTK_BUTTON(button));
	for(int i=0;i<cAno;i++){
		if(strcmp(label, commArd[i][1]) ==0){
			TCPMessage(commArd[i][0]);
			//g_print("%s\n", commArd[i][0]);
		}else if(i<cSno && strcmp(label, commSer[i][1]) ==0){
			TCPMessage(commSer[i][0]);
			//g_print("%s\n", commSer[i][0]);
		}
	}
	//g_print("%s\n", "Command response completed");
}

/*
	Callback function for responding to the arm position sliders or spin buttons on the Sliders tab
	@return void
	@param widget The widget from which the callback originated
	@param data The label associated with the slider
*/
static void slider_response(GtkWidget *widget, gpointer data){
	const char *label = gtk_label_get_text(GTK_LABEL(data));
	g_print("%s%s%s%f\n", 	"Slider ", 
							label,
							" changed to ", 
							gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget))
			);
	
	double value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	int multiplier = value>0?1:-1;
	
	value = (round(value * 100 *multiplier) / 100) * multiplier;
	
	for(int i=0;i<cASno;i++){
		if(strcmp(label, commArdS[i][1]) ==0){
			if(strcmp(commArdS[i][0], "aRot")==0){ // slider works in degrees, Arduino operates on Radians
				value = value / (180/M_PI);
			}else if((strcmp(commArdS[i][0], "aExt")==0) || (strcmp(commArdS[i][0], "vPos")==0) || (strcmp(commArdS[i][0], "hPos")==0)){ // slider works in cm, Arduino in mm
				value = value * 10;
			}
			char message[256];
			setlocale(LC_NUMERIC, "C"); // if the locale is set here it does work for sprintf, in main() it doesn't

			sprintf(message, "%s%s%f\n", commArdS[i][0], " ", value);
			
			g_print("%s\n",message);
			
			TCPMessage(message);
		}
	}
}

/*
	A function to create the sliders on the Slider tab. 
	This resulted in so much duplicate code that a function written for it made more sense.
	Can also create progress bars with the sliders, but this was disabled due to a server problem.
	@return GtkWidget The constructed widget that houses the new slider and spinbutton combination
	@param title The title of the slider
	@param unit The SI unit that is to be displayed next to the slider
	@param def The default value of the slider
	@param min The minimum value of the slider
	@param max The maximum value of the slider
*/
static GtkWidget* make_sliderBox(char* title, char* unit, int def, int min, int max){
	GtkWidget *hbox, *spacer, *vbox, *label, *spin, *scale;
	GtkObject *adjustment;
	
	vbox = gtk_vbox_new(false, 0);
	
	hbox = gtk_hbox_new(false, 0);
	spacer = gtk_label_new("");
	if(positionCheck) progressbar = gtk_progress_bar_new();
	
	gtk_widget_set_size_request(spacer, 10, 10);
	gtk_box_pack_start(GTK_BOX(hbox), spacer, false, false , 0);
	
	label = gtk_label_new(title);
	gtk_widget_set_size_request(label, 250, 25);
	
	gtk_box_pack_start(GTK_BOX(hbox), label, false, false, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, false, false, 0);
	
	hbox = gtk_hbox_new(false, 0);
	spacer = gtk_label_new("");
	gtk_widget_set_size_request(spacer, 10, 10);
	gtk_box_pack_start(GTK_BOX(hbox), spacer, false, false , 0);
	
	adjustment = gtk_adjustment_new(def, min, max, 1, 3, 0);
	scale=gtk_hscale_new(GTK_ADJUSTMENT(adjustment));
	gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_RIGHT);
	gtk_scale_set_digits(GTK_SCALE(scale), 1);
	gtk_scale_set_draw_value(GTK_SCALE(scale), false);
	gtk_widget_set_size_request(scale, 250, 25);
	
	spin = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment),1,1);
	gtk_widget_set_size_request(spin,65, 35);
	
	g_signal_connect(spin,"value-changed", G_CALLBACK(slider_response), label);
	label = gtk_label_new(unit);
	
	gtk_box_pack_start(GTK_BOX(hbox), scale, false, false, 0);
	gtk_box_pack_start(GTK_BOX(hbox), spin, false, false, 0);
	gtk_box_pack_start(GTK_BOX(hbox), label, false, false, 10);
	
	gtk_box_pack_start(GTK_BOX(vbox), hbox, false, false, 0);
	
	hbox = gtk_hbox_new(false, 0);
	spacer = gtk_label_new("");
	gtk_widget_set_size_request(spacer, 10, 10);
	gtk_box_pack_start(GTK_BOX(hbox), spacer, false, false , 0);
	
	gtk_box_pack_start(GTK_BOX(vbox), hbox, false, false, 0);
	
	hbox = gtk_hbox_new(false, 0);
	spacer = gtk_label_new("");
	gtk_widget_set_size_request(spacer, 10, 10);
	if(positionCheck) gtk_widget_set_size_request(progressbar, 250,15);
	double start;
	if(min<0){
		start = min*-1;
		start /= (start + max);
	}else{
		start = def;
	}
	
	if(positionCheck) gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar), start);
	gtk_box_pack_start(GTK_BOX(hbox), spacer, false, false , 0);
	if(positionCheck) gtk_box_pack_start(GTK_BOX(hbox), progressbar, false, false, 0);
	
	gtk_box_pack_start(GTK_BOX(vbox), hbox, false, false, 0);
	return vbox;
}

/*
	This function can be called to ask the server for the current position of the arm
	@return void	
*/
void check_position(){
	// return format: aExt(##.##) aRot(##.##) hPos(##.##) vPos(##.##)
	g_print("%s\n", "asking for position");

	TCPMessage(commArdS[4][0]);
	
	// updating progress bars has not been implemented yet since the old server PC couldn't handle the amount of requests. All returned data is stored in the posReturn char array
}

/*
	A function to that calls check_position every 1 second
	@return void
	@param arg I don't know what a void parameter is but it seems needed for the function
*/
void *threadproc(void *arg)
{
    while(!done)
    {
		if(positionCheck){
			sleep(1);
			check_position();
		}
    }
    return 0;
}

/*
	The main program, that creates the GUI.
	@return int A status message
	@param argc The amount of commandline arguments
	@param argv[] The commandline arguments. Passing these is optional, but the first argument is assumed to be the server address, second server port
*/
int main(int argc, char* argv[])
{
	if(positionCheck) {
		pthread_t tid;
		pthread_create(&tid, NULL, &threadproc, NULL);
	}

	if(argc > 1){
		cdet[0][0] = argv[1];  // server IP
	}else if(argc > 2){
		cdet[1][0] = argv[2]; // server port
	}
	
	gtk_init(&argc, &argv);
	GtkWidget *hbox, *button, *vbox, *spacer,
			  *menu_bar, *menu_item, *file_menu, *help_menu, 
		 	  *notebook, *label, *label2, *separator, 
		 	  *tempbox;
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
	
	vbox = gtk_vbox_new(false, false);
	spacer = gtk_label_new("");
	gtk_widget_set_size_request(spacer, 10, 10);
	gtk_box_pack_start(GTK_BOX(vbox), spacer, false, false , 0);
	
	notebook = gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook),GTK_POS_TOP);
	
/* Settings page */
	for(int i=0;i<2;i++){
		hbox = gtk_hbox_new(false, false);
		spacer = gtk_label_new("");
		gtk_widget_set_size_request(spacer, 10, 10);
		gtk_box_pack_start(GTK_BOX(hbox), spacer, false, false , 0);
		
		button = gtk_button_new_with_label(cdet[i][1]);
		gtk_widget_set_size_request(button, 140, 20);
		cdetIn[i] = gtk_entry_new();
		gtk_entry_set_text(GTK_ENTRY(cdetIn[i]), cdet[i][0]);
		
		gtk_box_pack_start(GTK_BOX(hbox), cdetIn[i], false, false, 0);
		gtk_box_pack_start(GTK_BOX(hbox), button, false, false, 0);
		g_signal_connect(button, "clicked", G_CALLBACK(setting_responseBtn), cdetIn[i]);
		g_signal_connect(cdetIn[i], "activate", G_CALLBACK(setting_responseEnt), button);
		gtk_box_pack_start(GTK_BOX(vbox), hbox, false, false, 0);
		
	}
	
	
	label = gtk_label_new("Settings");		
	
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),vbox, label);
/* End of settings */
	
/* Command page */
	vbox = gtk_vbox_new(false, 0);
	
	spacer = gtk_label_new("");
	gtk_widget_set_size_request(spacer, 10, 10);
	gtk_box_pack_start(GTK_BOX(vbox), spacer, false, false, 0);
		
	hbox = gtk_hbox_new(false, false);
	spacer = gtk_label_new("");
	gtk_widget_set_size_request(spacer, 10, 10);
	gtk_box_pack_start(GTK_BOX(hbox), spacer, false, false , 0);
	
	label = gtk_label_new("Arduino Commands");
	gtk_widget_set_size_request(label, 140, 25);
	label2 = gtk_label_new("Server Commands");
	gtk_widget_set_size_request(label2, 140, 25);
	
	gtk_box_pack_start(GTK_BOX(hbox), label, false, false , 0);
	gtk_box_pack_start(GTK_BOX(hbox), label2, false, false , 20);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, false, false, 0);
	
	hbox = gtk_hbox_new(false, false);
	separator = gtk_hseparator_new();
	gtk_widget_set_size_request(separator, 300, 10);
	
	spacer = gtk_label_new("");
	gtk_widget_set_size_request(spacer, 10, 10);
	gtk_box_pack_start(GTK_BOX(hbox), spacer, false, false , 0);

	gtk_box_pack_start(GTK_BOX(hbox), separator, false, false , 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, false, false, 0);
	
	for(int i=0;i < cAno;i++){
		hbox = gtk_hbox_new(false, false);
		
		spacer = gtk_label_new("");
		gtk_widget_set_size_request(spacer, 10, 10);
		gtk_box_pack_start(GTK_BOX(hbox), spacer, false, false , 0);
	
		button = gtk_button_new_with_label(commArd[i][1]);
		gtk_widget_set_size_request(button, 140, 35);
	
		gtk_box_pack_start(GTK_BOX(hbox), button, false, false, 0);
		g_signal_connect(button, "clicked", G_CALLBACK(command_response), &commArd[i][1]);

		if(i<cSno){
			button = gtk_button_new_with_label(commSer[i][1]);
			gtk_widget_set_size_request(button, 140, 35);
			
			gtk_box_pack_start(GTK_BOX(hbox), button, false, false, 20);
			g_signal_connect(button, "clicked", G_CALLBACK(command_response), &commSer[i][1]);
		}
	

		gtk_box_pack_start(GTK_BOX(vbox), hbox, false, false, 0);
	}
	
	label = gtk_label_new("Commands");		
	
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);
/* End of command */
	
/* Sliders */
	vbox = gtk_vbox_new(false, 0);
	spacer = gtk_label_new("");
	gtk_widget_set_size_request(spacer, 10, 10);
	gtk_box_pack_start(GTK_BOX(vbox), spacer, false, false , 0);

	// Extension
	
	tempbox = make_sliderBox((char *)commArdS[0][1], (char *)"cm", 0, 0, 60);
	gtk_box_pack_start(GTK_BOX(vbox), tempbox, false, false, 0);

	// Rotation
	
	tempbox = make_sliderBox((char *)commArdS[1][1], (char *)"degrees", 0, 0, 90);
	gtk_box_pack_start(GTK_BOX(vbox), tempbox, false, false, 0);

	// spacing
	hbox = gtk_hbox_new(false, 0);	
	spacer = gtk_label_new("");
	gtk_widget_set_size_request(spacer, 10, 10);
	gtk_box_pack_start(GTK_BOX(hbox), spacer, false, false , 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, false, false, 10);
	
	// horizontal position
	
	tempbox = make_sliderBox((char *)commArdS[2][1], (char *)"cm", 60, 60, 120); // TODO find out max of this
	gtk_box_pack_start(GTK_BOX(vbox), tempbox, false, false, 0); 
	
	// vertical position
	tempbox = make_sliderBox((char *)commArdS[3][1], (char *)"cm", 66, 66, 188); // TODO find out max of this
	gtk_box_pack_start(GTK_BOX(vbox), tempbox, false, false, 0); 
	
	label = gtk_label_new("Sliders");
	gtk_notebook_prepend_page(GTK_NOTEBOOK(notebook), vbox, label);
/* end of sliders */
	
	gtk_container_add(GTK_CONTAINER(window),notebook);
	gtk_widget_set_size_request(window, 500, 500);
	gtk_window_set_title(GTK_WINDOW(window), "JMP Client software, v1.1.0");
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}

