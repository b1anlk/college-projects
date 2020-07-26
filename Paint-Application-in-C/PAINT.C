#include<graphics.h>
#include<dos.h>
#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<conio.h>
union REGS i,o;
int leftcolor[15];

//BOTTOM PANEL
void draw_color_panel()
{
   int left, top, c, color;
   left= 100;
   top= 436;
   color= getcolor();
   setcolor(CYAN);
   rectangle(4,431,635,457);
   setcolor(RED);
   settextstyle(TRIPLEX_FONT,0,2);
   outtextxy(10,431,"Colors : ");

   for(c=1;c<=15;c++)
   {
      setfillstyle(SOLID_FILL,c);
      bar(left,top,left+16,top+16);
      leftcolor[c-1]= left;
      left+= 26;
   }
   setcolor(color);
}

//RIGHT PANEL
void draw_shape_panel()
{
   int left,top,c,color;
   left= 529;
   top= 45;
   color= getcolor();
   setcolor(YELLOW);
   rectangle(525,40,633,285);

   for(c=1;c<=8;c++)
   {
      rectangle(left,top,left+100,top+25);
      top+= 30;
   }
   setcolor(RED);
   outtextxy(530,45," !radial!");
   outtextxy(530,75," line");
   outtextxy(530,105," pixel");
   outtextxy(530,135," ellipse");
   outtextxy(530,165," freehand");
   outtextxy(530,195,"rectangle");
   outtextxy(530,255," clear");
   outtextxy(530,225," circle");
   setcolor(color);
}

void change_color(int x, int y)
{
   int c;
   for(c=0;c<=13;c++)
   {
      if(x>leftcolor[c] && x<leftcolor[c+1] && y>437 && y<453)
	 setcolor(c+1);
      if(x>leftcolor[14] && x<505 && y>437 && y<453)
	 setcolor(WHITE);
   }
}

char change_shape(int x, int y)
{
   if ( x>529 && x<625 && y>45 && y<70 )
      return 'b';
   else if ( x>529 && x<625 && y>75 && y<100 )
      return 'l';
   else if ( x>529 && x<625 && y>105 && y<130 )
      return 'p';
   else if ( x>529 && x<625 && y>135 && y<160 )
      return 'e';
   else if ( x>529 && x<625 && y>165 && y<190 )
      return 'f';
   else if ( x>529 && x<625 && y>195 && y<220 )
      return 'r';
   else if ( x>529 && x<625 && y>225 && y<250 )
      return 'z';
   else if ( x>529 && x<625 && y>255 && y<280 )
      return 'c';
   return 'f';
}

//DDA Algorithm start
int abs1(int n)
{
   return ((n>0) ? n : (n*(-1)));
}

void DDA(int X0,int Y0,int X1,int Y1)
{
   int i;
   int dx= X1-X0;
   int dy= Y1-Y0;
   int steps= abs1(dx) > abs1(dy) ? abs1(dx) : abs1(dy);
   float Xinc= dx/(float)steps;
   float Yinc= dy/(float)steps;
   float X= X0;
   float Y= Y0;
   for(i=0;i<=steps;i++)
   {
      putpixel(X,Y,getcolor());
      X+= Xinc;
      Y+= Yinc;
   }
}

//Mid point circle algorithm
void plot(int x,int y,int xc,int yc)
{
   putpixel(xc+x,yc-y,getcolor());
   putpixel(xc+x,yc+y,getcolor());
   putpixel(xc-x,yc-y,getcolor());
   putpixel(xc-x,yc+y,getcolor());
   putpixel(xc+y,yc-x,getcolor());
   putpixel(xc+y,yc+x,getcolor());
   putpixel(xc-y,yc-x,getcolor());
   putpixel(xc-y,yc+x,getcolor());
}

void mcircle(int x0,int y0,int x2,int y2)
{
   int r,d,xc,yc,x1,y1;
   float pk;
   d=(int)sqrt(pow((x0-x2),2)+pow((y0-y2),2));
   r=(int)d/2;
   xc=(int)(x0+x2)/2;
   yc=(int)(y0+y2)/2;
   x1=0;
   y1=r;
   pk=(1.25-r);
   plot(x1,y1,xc,yc);
   while(x1<y1)
   {
      if(pk<0)
      {
	 pk=pk+(2*(x1))+3;
	 x1=x1+1;
      }
      else
      {
	 pk=pk+(2*(x1-y1))+5;
	 x1=x1+1;
	 y1=y1-1;
      }
      plot(x1,y1,xc,yc);
   }
}

//mouse lmao
int get_key()
{
   union REGS i,o;
   i.h.ah = 0;
   int86(22, &i, &o);
   return ( o.h.ah );
}

void showmouseptr()
{
   i.x.ax = 1;
   int86(0x33, &i, &o);
}

void hidemouseptr()
{
   i.x.ax = 2;
   int86(0x33, &i, &o);
}

void restrictmouseptr( int x1, int y1, int x2, int y2)
{
   i.x.ax = 7;
   i.x.cx = x1;
   i.x.dx = x2;
   int86(0x33, &i, &o);

   i.x.ax = 8;
   i.x.cx = y1;
   i.x.dx = y2;
   int86(0x33, &i, &o);
}

void getmousepos(int *button,int *x,int *y)
{
   i.x.ax = 3;
   int86(0x33, &i, &o);

   *button = o.x.bx;
   *x = o.x.cx;
   *y = o.x.dx;
}

//main[fill,radial]
int main()
{
   int gd= DETECT,gm;

   int maxx,maxy,x,y,button,prevx,prevy,rx,ry,temp1,temp2,color,key;
   char ch= 'f' ;                // default free-hand drawing!

   initgraph(&gd,&gm,"C:\\TC\\BGI");

   maxx= getmaxx();
   maxy= getmaxy();

   setcolor(BLUE);
   rectangle(0,0,maxx,maxy);

   setcolor(WHITE);
   settextstyle(SANS_SERIF_FONT,HORIZ_DIR,2);
   outtextxy(maxx/2-180,maxy-28,"Test Phase 68/Nikhil/");

   draw_color_panel();
   draw_shape_panel();

   setviewport(1,1,maxx-1,maxy-1,1);

   restrictmouseptr(1,1,maxx-1,maxy-1);
   showmouseptr();
   rectangle(2,2,518,426);
   setviewport(1,1,519,427,1);

   while(1)
   {
      if(kbhit())
      {
	 key= get_key();
	 if(key==1)
	 {
	    closegraph();
	    exit(0);
	 }
      }

      getmousepos(&button,&x,&y);

      if(button==1)
      {
	 if( x>4 && x<635 && y>431 && y<457 )
	    change_color(x,y);
	 else if ( x>529 && x<625 && y>40 && y<280 )
	    ch= change_shape(x,y);

	 temp1= x;
	 temp2= y;

	 if(ch=='f')
	 {
	    hidemouseptr();
	    while(button==1)
	    {
	       line(temp1,temp2,x,y);
	       temp1= x;
	       temp2= y;
	       getmousepos(&button,&x,&y);
	    }
	    showmouseptr();
	 }

	 while(button==1)
	    getmousepos(&button,&x,&y);

	    /* to avoid interference of mouse while drawing */
	 hidemouseptr();

	 if(ch=='p')
	    putpixel(x,y,getcolor());
	 else if (ch=='z')
	    mcircle(temp1,temp2,x,y);
	 else if (ch=='b')
	    DDA(temp1,temp2,x,y);//LMAOO radial not works :(
	 else if (ch=='l')
	    DDA(temp1,temp2,x,y);
	 else if (ch=='e')
	    ellipse(temp1,temp2,0,360,abs(x-temp1),abs(y-temp2));
	 else if (ch=='r')
	    rectangle(temp1,temp2,x,y);
	 else if (ch=='c')
	 {
	    ch='f';            //since default was freehand!
	    clearviewport();
	    color= getcolor();
	    setcolor(WHITE);
	    rectangle(2,2,518,427);
	    setcolor(color);
	 }
	 showmouseptr();
      }
   }
   return 0;
}