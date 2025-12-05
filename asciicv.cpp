#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ncurses.h>
#include <opencv2/opencv.hpp>

constexpr char density[] = {" `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@"};

char convertChar(const int x){
	return density[int(((double)x/255)*92)];}

void write(const char* a, const int arow, const int acol, char** ascii){
	std::fstream outfile(a, std::ios::trunc | std::ios::out);
	if (!outfile.is_open()){
		std::cout<<"Failed to open the file for writing\n";
		return;}
	for (int i = 0; i < arow; i++){
		for (int j = 0; j < acol; j++){
			outfile << ascii[i][j];}
		outfile << "\n";}
	outfile.close();}

int findMin(const int x, const int y){
	if (x>y){
		return y;}
	else {
		return x;}}

//stuff to render in terminal window, pretty simple logic
void renderImage(const int top_line, const int left_line, const int row, const int arow, const int col, const int acol, char** ascii){

	const int new_r = findMin(top_line+row, arow);
	const int new_c = findMin(left_line+col, acol);

	for (int i = top_line; i < new_r; i++){
		move(i - top_line, 0);
		for (int j = left_line; j < new_c; j++){
			addch(ascii[i][j]);
		}
	}
	refresh();
}

int main(int argc, char *argv[]){
	
	char* directory = NULL;
	char* savename = NULL;
	int newh=50;
	
	if (argc<3){
		std::cout<<
		"\nImage to ascii converter\n\n"<<
		"Usage: <path> <height[px]> [save_file_name]\n\n"<<
		"While in window:\n"<<
		"k/j: scroll up/down\n"<<
		"h/l: scroll left/right\n"<<
		"q: quit\n"<<
		"s: save\n\n";
		return 0;
		}
		
	else if (argc==3){
		directory = argv[1];
	 	newh = std::atoi(argv[2]);
		if (newh == 0){
			std::cout<<"\nError: height is not valid \n"<<
				   "Choose an int value that will represent h in pixels\n";
				   return 1;}
		}
	else if (argc==4){
		directory = argv[1];
	 	newh = std::atoi(argv[2]);
		savename = argv[3];
		}

	else	{ 
		std::cout<<"\nIncorrect arguments\n\n"<<
		"Usage: <path> <height[px]> [save_file_name]\n"<<std::endl;
		return 1;}

/////////////////////////////////////////////////
	int row, col; //screen dimensions
	
	initscr();
	raw();
	noecho();
	keypad(stdscr, TRUE);
	scrollok(stdscr, TRUE);

	//read image, convert to 3 channels
	cv::Mat image = cv::imread(directory,cv::IMREAD_COLOR);

	if (image.empty()) {
		getmaxyx(stdscr,row,col);
		mvprintw(row/2, (col-strlen("file could not be read:("))/2,"%s","file could not be read:(");
		refresh();
		getch();
		endwin();
		return 1;}
	
	//downscale image based on arg provided
	const int neww = ((newh*image.size().width)/image.size().height)*1.6; 

	cv::Mat temp_image;
	cv::resize(image, temp_image, cv::Size(neww, newh));
	image = temp_image;

	const int acol = image.size().width;
	const int arow = image.size().height;

	char** ascii = new char*[arow];
	for (int i=0; i < arow; i++){
		ascii[i] = new char[acol];}

	//iterate through pixels, incrementation by 3 as the image will always have 3 channels. We will always end up on the index that corresponds to the r value of a pixel. 
	
	for (int j=0; j < image.size().height; j++){
		for (int i=0; i < image.size().width; i++){
			const int pindex = (i + j * image.size().width) * 3;
			const int r = image.at<uchar>(pindex);
			const int g = image.at<uchar>(pindex+1);
			const int b = image.at<uchar>(pindex+2);
			const int  avg = ( r + b + g)/3;

			ascii[j][i]=convertChar(avg);
		}
	}
	if (argc==4){
		refresh();
		endwin();
		write(savename, arow, acol, ascii);
		return 0;}

	//if image is too large, this section allows to naviate it using h j k l
	
	int top_line=0;
	int left_line=0;
	int ch;

	getmaxyx(stdscr,row,col);
	renderImage(top_line,left_line, row, arow, col, acol, ascii);
	
	while ((ch = getch()) != 'q'){
		getmaxyx(stdscr,row,col);
		if (ch =='k'){
			if (!((top_line-5) < 0)){
			clear();
			top_line=top_line-5;
			renderImage(top_line,left_line, row, arow, col, acol, ascii);}}
		if (ch =='j'){
			if (!((top_line+5) >= arow-row)){
			clear();
			top_line=top_line+5;
			renderImage(top_line,left_line, row, arow, col, acol, ascii);}}
		if (ch =='l'){
			if (!((left_line+5) >= acol-col)){
			clear();
			left_line=left_line+5;
			renderImage(top_line,left_line, row, arow, col, acol, ascii);}}
		if (ch =='h'){
			if (!((left_line-5) < 0)){
			clear();
			left_line=left_line-5;
			renderImage(top_line,left_line, row, arow, col, acol, ascii);}}
		if (ch =='s'){
			char name[255];
			refresh();
			endwin();
			std::cout <<"\nProvide file name: "<<std::endl;
			std::cin.getline(name,255);
			std::fstream outfile(name, std::ios::trunc | std::ios::out);
			if (!outfile.is_open()){
				std::cout<<"failed to open/create file"<<std::endl;}
			for (int i = 0; i < arow; i++){
				for (int j = 0; j < acol; j++){
					outfile << ascii[i][j];}
				outfile << "\n";}
			outfile.close();
			break;}

			

	}

	for (int i=0; i<arow; i++){
		delete[] ascii[i];}
	delete[] ascii;
	
	refresh();
	endwin();

	return 0;
}
