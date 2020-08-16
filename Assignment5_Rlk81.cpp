//Ralph Kelley
// Assignment 5


#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include<stdio.h>
#include <fstream>
//<<<<<<<<<<<<<<<<<<<<< Datatypes & Variables  >>>>>>>>>>>>>>>>>>
struct Image
   {
       unsigned char* pixels;
       int width;
       int height;
       int numChannels;
   };

   class Texture
   {
       public:
       Texture ();
       void Prepare (int texN);

       void ReadPPMImage (const char *fn);

       GLuint texName;
       Image image;
   };

   const float fMinX = -5.0, fMinY = -5.0, fNearZ = 1.0,
           fMaxX = 5.0 , fMaxY = 5.0 , fFarZ = 10.0;

Texture ImageOne, ImageTwo, ImageThree, ImageFour, ImageFive, ImageSix, ImageSeven ;

GLboolean bRotate = false, uRotate = false;
GLfloat alpha = 0.0, theta = 0.0, axis_x=0.0, axis_y=0.0;


float ex=2,ey=2,ez=9,lx=2,ly=2,lz=8;

GLfloat diffuse[]    = { 0.7, 0.8, 0.7, 1.0};
GLfloat ambient[]    = { 0.4f, 0.2f, 0.3f, 1.0f  };
GLfloat shininess[] = { 4.0f };
GLfloat specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
bool ambient_on = true;
bool specular_on = true;
bool diffuse_on = true;

bool light_1_on = true;

float constant_attenuation = 2.3f;
float linear_attenuation = 0.01f;

GLfloat light_position[] = { 1.0,1.5,1.0, 1.0 };
GLfloat mat_zero[] = {0.0f, 0.0f, 0.0f, 1.0};

float cam_angle = 270/180.0f * 3.1416;
float mm,xx,yy,zz;

bool a11=true,d11=true,s11=true;
bool a21=true,d21=true,s21=true;
bool a12=true,d12=true,s12=true;
bool a22=true,d22=true,s22=true;
//<<<<<<<<<<<<<<<<<<<<< Prototypes >>>>>>>>>>>>>>>>>>
Texture::Texture ()
{
}

void Texture::Prepare (int texN)
{
    texName = texN;

    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

    glBindTexture (GL_TEXTURE_2D, texName);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width,
                    image.height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                    image.pixels);
}

void Texture::ReadPPMImage (const char* fn)
{
    int tmpint;
    char str[100];
    FILE* inFile = fopen (fn,"rb");

    if (inFile == NULL)
    {
        printf ("Can't open input file %s. Exiting.\n",fn);
        exit (1);
    }

    fscanf (inFile,"P%d\n", &tmpint);

    if (tmpint != 6)
    {
        printf ("Input file is not ppm. Exiting.\n");
        exit (1);
    }

    // skip comments embedded in header

    fgets (str,100,inFile);
    while (str[0]=='#')
       fgets(str,100,inFile);

    // read image dimensions

    sscanf (str,"%d %d",&image.width, &image.height);
    fgets (str,100,inFile);
    sscanf (str,"%d",&tmpint);

    if (tmpint != 255)
        printf("Warning: maxvalue is not 255 in ppm file\n");

    image.numChannels = 3;
    image.pixels = (unsigned char*) malloc (image.numChannels * image.width *  image.height * sizeof (unsigned char));

    if (image.pixels == NULL)
    {
        printf ("Can't allocate image of size %dx%d. Exiting\n", image.width, image.height);
        exit (1);
    }
    else
        printf("Reading image %s of size %dx%d\n", fn, image.width, image.height);


    fread (image.pixels, sizeof (unsigned char), image.numChannels * image.width * image.height, inFile);

    fclose (inFile);
}
//<<<<<<<<<<<<<<<<<<<<<<< myInit >>>>>>>>>>>>>>>>>>>>
void Init ()
{
  glClearColor (0.0, 0.0, 0.0, 0.0);
  glEnable (GL_DEPTH_TEST);
    glGenTextures (1, &ImageOne.texName);
    ImageOne.ReadPPMImage("Cloud.ppm");
    ImageOne.Prepare(1) ;

    glGenTextures (2, &ImageTwo.texName);
    ImageTwo.ReadPPMImage("Fractal.ppm");
    ImageTwo.Prepare(2) ;

    glGenTextures (3, &ImageThree.texName);
    ImageThree.ReadPPMImage("Desert.ppm");
    ImageThree.Prepare(3) ;

    glGenTextures (4, &ImageFour.texName);
    ImageThree.ReadPPMImage("Desert.ppm");
    ImageThree.Prepare(4) ;

    glGenTextures (5, &ImageFive.texName);
    ImageThree.ReadPPMImage("Volc.ppm");
    ImageThree.Prepare(5) ;

    glGenTextures (6, &ImageSix.texName);
    ImageThree.ReadPPMImage("Jungle.ppm");
    ImageThree.Prepare(6) ;
}

void set_material(GLfloat* ambient_col, GLfloat* diffuse_col, GLfloat* specular_col, GLfloat* shininess)
{
  if (ambient_on == true)
      glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_col);
  else
      glMaterialfv(GL_FRONT, GL_AMBIENT, mat_zero);

  if (diffuse_on == true)
      glMaterialfv(GL_FRONT, GL_DIFFUSE,  diffuse_col);
  else
      glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_zero);

  if (specular_on == true)
      glMaterialfv(GL_FRONT, GL_SPECULAR,  specular_col);
  else
      glMaterialfv(GL_FRONT, GL_SPECULAR, mat_zero);

      glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

}

void setLight(int light_id, bool light_on, GLfloat* ambient, GLfloat* diffuse, GLfloat* specular, GLfloat* position)
{
  int id;
  if (light_id == 0)
      id = GL_LIGHT0;

  if (light_on == true)
      glEnable(id);
  else
      glDisable(id);

  if (a11 == true)
      glLightfv(id, GL_AMBIENT,  ambient);
  else
      glLightfv(id, GL_AMBIENT,  mat_zero);

  if (d11 == true)
      glLightfv(id, GL_DIFFUSE,  diffuse);
  else
      glLightfv(id, GL_DIFFUSE,  mat_zero);

  if (s11 == true)
      glLightfv(id, GL_SPECULAR, specular);
  else
      glLightfv(id, GL_SPECULAR, mat_zero);

  glLightfv(id, GL_POSITION, position);

  glLightf(id, GL_LINEAR_ATTENUATION, linear_attenuation );
  glLightf(id,GL_CONSTANT_ATTENUATION, constant_attenuation);
}

//<<<<<<<<<<<<<<<<<<<<<<<< myDisplay >>>>>>>>>>>>>>>>>
void drawRoom(float x, float y , float z)
{
  glPushMatrix();

      GLfloat diffuse7[] = {0,0.5,0.2,1};
  set_material(ambient,diffuse7,specular,shininess);
  glTranslatef(x+2.5,y+0.025,z+2.5);
  glScalef(5,0.05*9,5);
  glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
      GLfloat diffuse8[] = {0.439, 1,0,1};
  set_material(ambient,diffuse8,specular,shininess);
  glTranslatef(x+0.025,y+1.5,z+2.5);
  glScalef(0.05,3,5);
  glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
      GLfloat diffuse9[] = {1, 0.837, 0,1};
  set_material(ambient,diffuse9,specular,shininess);
  glTranslatef(x+2.5,y+1.5,z+0.025);
  glScalef(5,3,0.05);
  glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
      GLfloat diffuse10[] = {1, 0.278, 0,1};
  set_material(ambient,diffuse10,specular,shininess);
  glTranslatef(x+2.5,y+3.025,z+2.5);
  glScalef(5,0.05,5);
  glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
      GLfloat diffuse11[] = {0.4, 0.802, 0.67,1};
  set_material(ambient,diffuse11,specular,shininess);
  glTranslatef(x+4.95,y+1.5,z+2.5);
  glScalef(0.05,3,5);
  glutSolidCube(1);
  glPopMatrix();

}

void drawCubeSmall(float x, float y , float z)
{
  glPushMatrix();

  glTranslatef(x,y+.5,z);

  glEnable (GL_TEXTURE_2D);
  glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glBindTexture (GL_TEXTURE_2D, ImageFive.texName);

  //  glShadeModel(GL_SMOOTH);

    glPushMatrix();
    glTranslatef(1,1,1);
  glRotatef(44, 0, 0, 1);

  glBegin(GL_QUADS);
      glTexCoord2f(0,1);
      glVertex3f(-0.25f,.25f,-.25f);
          glTexCoord2f(0,0);
      glVertex3f(-.25f,-.25f,-.25f);
      glTexCoord2f(1,0);
      glVertex3f(.25f,-.25f,-.25f);
      glTexCoord2f(1,1);
      glVertex3f(.25f,.25f,-.25f);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

void drawPoint() { //draws point done

    glPushMatrix();
    glColor3f(0.4f, 0.6f, 0.4f);          // set the drawing color

    glPointSize(12.0);
    glEnable (GL_TEXTURE_2D);
    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture (GL_TEXTURE_2D, ImageFour.texName);

    glBegin(GL_POINTS);

    GLfloat diffuse1[] = {0.662745, 0.662745, 0.662745,1};
    set_material(ambient,diffuse1,specular,shininess);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex3f(2.5, 2.0, 3.0);

     glEnd();
         glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawLine() { //draws line done

  glPushMatrix();
  glLineWidth(10.0);
  glShadeModel(GL_FLAT);

            //line materials
            glEnable (GL_TEXTURE_2D);
            glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            glBindTexture (GL_TEXTURE_2D, ImageThree.texName);


        glBegin(GL_LINE_STRIP);

glTexCoord2f(0,0.5);
            glVertex3f(0.5, 2.0, 0.5);
          glTexCoord2f(2.0,0);
            glVertex3f(0.5, 0.0, 1.5);

            GLfloat diffuse5[] = {1, 0, 0,1};
            set_material(ambient,diffuse5,specular,shininess);

    glEnd();
    glDisable(GL_TEXTURE_2D);
      glPopMatrix();
}

void drawTriangle(){ //draws triangle done


    glPushMatrix();

    glTranslatef(2.45,1.39,3.3);

    glRotatef(45, 0, 0, 1);

  //  glShadeModel(GL_FLAT);

    glEnable (GL_TEXTURE_2D);
    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture (GL_TEXTURE_2D, ImageTwo.texName);


        glBegin(GL_POLYGON);
        GLfloat diffuse5[] = {1, 0, 0,1};
        set_material(ambient,diffuse5,specular,shininess);

        //glColor3f(1.0f,1.0f,0.0f);   // Color Yellow
        glTexCoord2f(0,0.5);
        glVertex3f(-0.5, -0.5, 0.0);

        glTexCoord2f(0,0);
        glVertex3f(0.5, -0.5, 0.0);

        glTexCoord2f(.5,0.5);
        glVertex3f(0.0, 0.5, 0.0);

    glEnd();



      glPopMatrix();

}

void drawSquare() { //draws square done
    glPushMatrix();
    glTranslatef(4,1.3,2);
  //  glScalef(.75,.75,.75);
    glRotatef(270, 0, 0, 1);

    glColor3f(0.0f,1.0f,0.5f);

    //square materials
    glEnable (GL_TEXTURE_2D);
    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture (GL_TEXTURE_2D, ImageOne.texName);

        glBegin(GL_POLYGON);
        glTexCoord2f(0,1);
        glVertex3f(-0.5, -0.5, 0.0);

      glTexCoord2f(0,0);
      glVertex3f(-0.5, 0.5, 0.0);

      glTexCoord2f(1,0);
      glVertex3f(0.5, 0.5, 0.0);

      glTexCoord2f(1,1);
      glVertex3f(0.5, -0.5, 0.0);


    glEnd(); // end of rectangle quad

    glDisable(GL_TEXTURE_2D);
      glPopMatrix();
}

void drawHexagon(){ //draws hexagon

        glPushMatrix();
        glTranslatef(1.6, 0.7, 1.5);
    //    glScalef(1.5,.5,.5);
    //    glRotatef(30,15, 0, 1);

        glShadeModel(GL_FLAT);

        glEnable (GL_TEXTURE_2D);
        glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glBindTexture (GL_TEXTURE_2D, ImageSix.texName);


           glBegin(GL_POLYGON);

  for(int i = 0; i <= 360; i+=60){
      double x = cos(i* (M_PI/180))/2;
      double y = sin(i* (M_PI/180))/2;
      glVertex3d(x,y,0.0);
    }

    glEnd();
    glDisable(GL_TEXTURE_2D);
      glPopMatrix();

}

void drawSphere(double r, int lats, int longs) {
    glPushMatrix();
    glTranslatef(1.0, 1.52, -0.75);
  //  glScalef(1.25,1.0,.75);
    glColor3f(0.0f, 1.0f, 0.0f);

    glShadeModel(GL_SMOOTH);

    glPushMatrix();

    int i, j;
    for(i = 0; i <= lats; i++) {
        double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
        double z0  = sin(lat0);
        double zr0 =  cos(lat0);

        double lat1 = M_PI * (-0.5 + (double) i / lats);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for(j = 0; j <= longs; j++) {
            double lng = 2 * M_PI * (double) (j - 1) / longs;
            double x = cos(lng);
            double y = sin(lng);

            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(x * zr0, y * zr0, z0);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(x * zr1, y * zr1, z1);
        }

        glEnd();
glDisable(GL_TEXTURE_2D);
    }
    glPopMatrix();
    glFlush();


}

void drawCircle() { //draws circle
    glPushMatrix();
    glTranslatef(3.25,1.0,3.65);
  //  glScalef(.75,1.25,1.25);
    glRotatef(225, 0, 0, 1);

glShadeModel(GL_FLAT);
glEnable (GL_TEXTURE_2D);
glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
glBindTexture (GL_TEXTURE_2D, ImageOne.texName);
  glBegin(GL_POLYGON);
//  glColor3f(1.0f, 0.5f, 0.2f);          // set the drawing color

  for(int i =0; i <= 360; i++){
      double x = cos(i*(M_PI/180))/2;
      double y = sin(i*(M_PI/180))/2;
      glNormal3f(0,0,1);
        glTexCoord2f(1,0);
      glVertex3d(x,y,0.0);
  }

    glEnd();
      glPopMatrix();

}
//<<<<<<<<<<<<<<<<<<<<<<<< myDisplay >>>>>>>>>>>>>>>>>
void myDisplay(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  gluPerspective(65,1,1,100);

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  gluLookAt(ex,ey,ez, lx,ly,lz, 0,1,0);

  glViewport(400,400,400,400);
  glPushMatrix();
  GLfloat d3[] = {1,1,1,1};
  setLight(0,true,ambient,d3,specular,light_position);
  glRotatef( theta, 1, 0, 0.0 );
  glRotatef( alpha, 0, 1, 0.0 );
  drawRoom(0,0,0);

  drawCubeSmall(1,0,4);
drawPoint();
drawLine();
drawTriangle();
drawSquare();
drawHexagon();
drawCircle();
drawSphere(1.0, 20, 15);


  glPopMatrix();



  glFlush();
  glutSwapBuffers();
}
//<<<<<<<<<<<<<<<<<<<<<<<< myKeyboard >>>>>>>>>>>>>>

void myKeyboard( unsigned char theKey, int x, int y )
{
  switch ( theKey )
  {
    //controls the camera
  case 'a':
      cam_angle = cam_angle - 0.01f;

      lx = ex + cos(cam_angle);
      ly = ey;
      lz = ez + sin(cam_angle);

      break;
  case 'd':
      cam_angle = cam_angle + 0.01f;

      lx = ex + cos(cam_angle);
      ly = ey;
      lz = ez + sin(cam_angle);

      break;
  case 'w':
      ex = ex + cos(cam_angle) * 0.5f;
      ez = ez + sin(cam_angle) * 0.5f;

      lx = ex + cos(cam_angle);
      ly = ey;
      lz = ez + sin(cam_angle);
      break;
  case 's':
      ex = ex - cos(cam_angle) * 0.5f;
      ez = ez - sin(cam_angle) * 0.5f;

      lx = ex + cos(cam_angle);
      ly = ey;
      lz = ez + sin(cam_angle);

      break;

  case 'q':	// Escape theKey
      exit(1);
  }
}

void myAnimate(){
  if (bRotate == true)
  {
      theta += 0.2;
      if(theta > 360.0)
          theta -= 360.0*floor(theta/360.0);
  }

  if (uRotate == true)
  {
      alpha += 0.2;
      if(alpha > 360.0)
          alpha -= 360.0*floor(alpha/360.0);
  }
  glutPostRedisplay();

}
//<<<<<<<<<<<<<<<<<<<<<<<< main >>>>>>>>>>>>>>>>>>>>>>
int main (int argc, char **argv){
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // set display mode
  glutInitWindowSize(1200,1200); // define a window
  glutInitWindowPosition(0,0); // initializing left corner for no-overlap

glutCreateWindow("Ralph Kelley-Project 5") ; // open the screen window

    Init ();

  glShadeModel( GL_SMOOTH );
  glEnable( GL_DEPTH_TEST );
  glEnable(GL_NORMALIZE);

  glutKeyboardFunc(myKeyboard); // register the keyboard action function
  glutDisplayFunc(myDisplay);  // register redraw function
  glutIdleFunc(myAnimate);

  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glutMainLoop();   // go into a perpetual loop

  return 0;
}
