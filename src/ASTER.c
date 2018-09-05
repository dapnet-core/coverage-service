
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <x86_64-linux-gnu/sys/io.h>
#include <x86_64-linux-gnu/sys/stat.h>
#include <unistd.h>


struct data
{

    int  max_north, n, max_east, min_east,
            min_north, min_elevation, max, max2;

};

int aster[3601][3601];

int ReadASTER(char *filename, char * path_dem, data &data1)
{
        int x, y, infile,bytes_read,  byte=0;
        unsigned char error, buffer[20];
        char north[4],east[5],*base =NULL, *base2=0;



    if ((strstr(filename, ".tif") == NULL))
       {
           return 0;
       }


    //Fileformat: ASTGTM2_N49E006_dem.tif

    base=strrchr(filename,  '/' );

    base +=1;
    if (base==NULL)
     base=filename;

    base2 = strstr(base, "_" );

            base2+=1;

            north[0]=base2[1];
            north[1]=base2[2];
            north[2]=0;

            east[0]=base2[4];
            east[1]=base2[5];
            east[2]=base2[6];
            east[3]=0;

            if ((base2[0]!='N' && base2[0]!='S') || (base2[3]!='W' && base2[3]!='E'))
            {
                    return 0;
            }

/*
int atoi (const char * str);
Convert string to integer
Parses the C-string str interpreting its content as an integral number, which is returned as a value of type int.
*/

          data1.min_east=atoi(east);

    if (base2[3]=='W')
            {
             data1.min_east= 360 - data1.min_east;
             data1.max_east=data1.min_east+1;
            }
     if (base2[3]=='E')
            {
             data1.max_east=data1.min_east+1;
            }
/*     if (data1.max_east> 180)
            {
             data1.max_east= data1.max_east - 360;
             data1.min_east = data1.max_east -1;
            }
     if(data1.min_east < -179)
            {
            data1.min_east= 360 + data1.min_east;
            data1.max_east= data1.min_east +1;
            }
*/

            if (base2[0]=='N')
                    data1.min_north=atoi(north);
            else
            {
             printf("No Coordinates in the south of the equator available \n");

                    return 0;
            }
            data1.max_north=data1.min_north+1;




/*
LOW LEVEL IO - TO READ 16Bit Intgeger from tif File
open()


        #include <fcntl.h>
        int  open(  char  *filename,  int  access,  int  permission  );

The available access modes are


        O_RDONLY		O_WRONLY		O_RDWR
        O_APPEND		O_BINARY		O_TEXT

The permissions are


        S_IWRITE	S_IREAD	S_IWRITE | S_IREAD

The open() function returns an integer value, which is used to refer to the file. If un- successful, it returns -1, and sets the global variable errno to indicate the error type.

read()


        #include  <fcntl.h>
        int  read(  int  handle,  void  *buffer,  int  nbyte );

The read() function attempts to read nbytes from the file associated with handle, and places the characters read into buffer. If the file is opened using O_TEXT, it removes carriage returns and detects the end of the file.

The function returns the number of bytes read. On end-of-file, 0 is returned, on error it returns -1, setting errno to indicate the type of error that occurred.

write()


        #include  <fcntl.h>
        int  write(  int  handle,  void  *buffer,  int  nbyte  );

The write() function attempts to write nbytes from buffer to the file associated with handle. On text files, it expands each LF to a CR/LF.

The function returns the number of bytes written to the file. A return value of -1 indicates an error, with errno set appropriately.

close()


        #include  <fcntl.h>
        int  close(  int  handle  );

The close() function closes the file associated with handle. The function returns 0 if successful, -1 to indicate an error, with errno set appropriately.	*/


    infile=open(filename, O_RDWR);

    if (infile < 0)
        {
        return 0;
        }

        read(infile,&buffer,2);

        //Deskriptor auf Dateianfang setzen
        lseek(infile,0L,SEEK_SET);

        //Tiff Header überlesen
         bytes_read=read(infile,&buffer,8);   //-> 8 Byte werden gelesen

        error=0;

        printf("Reading %s... ", filename);
        fflush(stdout);

        for (x=0; (x<=data1.max && error==0); x++)
        {for (y=0; (y<=data1.max && error==0); y++)
                {
                        bytes_read=read(infile,&buffer,2);   //-> 2 Byte werden gelesen

                        if (bytes_read==2)
                        {
                           /* "little-endian" structure */


                                        byte=buffer[0]+(buffer[1]<<8);  //-> Shift um 8 Bit, in ASTER sind es 16 Bit Integer; read liest 2 8Bit Integer aus der Datei

                                        if (buffer[1]&128)
                                                byte-=0x10000;
                                if (byte<-32768)
                                        byte=-32768;

                                if (byte>32767)
                                        byte=32767;

                                aster[x][y]=byte;




                        }
                        else
                                error=1;
                }
    }

        if (error)
        {
                fprintf(stdout,"\nFehler beim Lesen der ASTER Daten");
                return 0;
        }

        close(infile);

    return 1;
}

void average_aster(int y,int x,int last_good, data &data1)
{

        long accum;
        int temp=0, count, bad_value;
        double average;

        bad_value = aster[y][x];

        accum=0L;
        count=0;

        if (y>=2)
        {
                temp=aster[y-1][x];

                if (temp>bad_value)
                {
                        accum+=temp;
                        count++;
                }
        }

        if (y<=data1.max2)
        {
                temp=aster[y+1][x];

                if (temp>bad_value)
                {
                        accum+=temp;
                        count++;
                }
        }

        if ((y>=2) && (x<=(data1.max2-1)))
        {
                temp=aster[y-1][x+1];

                if (temp>bad_value)
                {
                        accum+=temp;
                        count++;
                }
        }

        if (x<=(data1.max2-1))
        {
                temp=aster[y][x+1];

                if (temp>bad_value)
                {
                        accum+=temp;
                        count++;
                }
        }

        if ((x<=(data1.max2-1)) && (y<=data1.max2))
        {
                temp=aster[y+1][x+1];

                if (temp>bad_value)
                {
                        accum+=temp;
                        count++;
                }
        }

        if ((x>=1) && (y>=2))
        {
                temp=aster[y-1][x-1];

                if (temp>bad_value)
                {
                        accum+=temp;
                        count++;
                }
        }

        if (x>=1)
        {
                temp=aster[y][x-1];

                if (temp>bad_value)
                {
                        accum+=temp;
                        count++;
                }
        }

        if ((y<=data1.max2) && (x>=1))
        {
                temp=aster[y+1][x-1];

                if (temp>bad_value)
                {
                        accum+=temp;
                        count++;
                }
        }

        if (count!=0)
        {
                average = (((double)accum)/((double)count));
                temp=(int)(average+0.5);
        }

        if (temp>data1.min_elevation)

            aster[y][x]=temp;
        else
                aster[y][x]=data1.min_elevation;
}

int WriteDEM1(char* filename, data &data1)
{

        int x, y, byte, last_good_byte=0;
        FILE *outfile;

        printf("\nWriting %s... ", filename);
        fflush(stdout);

        outfile=fopen(filename,"wb");
    if(outfile <0)
    {
    cout << ".dem File could not be created";
     return 0;
    }

        fprintf(outfile, "%d\n%d\n%d\n%d\n", data1.min_east, data1.min_north, data1.max_east, data1.max_north);

        for (y=data1.max; y>=1; y--)		/* Omit the northern most edge */
                {
                    for (x=data1.max; x>=1; x--) /* Omit the western most edge  */
            {
                byte=aster[y][x];


                if (byte>=data1.min_elevation)
                    last_good_byte=byte;

                //Fehlerbehandlung für fehlende Pixel
                if (byte<data1.min_elevation)
                {
                                        average_aster(y,x,last_good_byte,data1);
                                        fprintf(outfile,"%d",aster[y][x]);
                                        fprintf(outfile," ");

                }
                else
                fprintf(outfile,"%d",byte);
                fprintf(outfile," ");
                fflush(outfile);

            }
            fprintf(outfile, "\n");
            fflush(outfile);
    }
            printf("Done!\n");
            return 1;

        fclose(outfile);
}

int ASTER(char* path_aster, char* path_dem1)
{

    data data1;
    data1.max = 3600;
    data1.max2 = data1.max -1;
    data1.min_elevation = 0;

    int success = 0;

    //ReadASTER needs full path with .tif extend and

        if (ReadASTER(path_aster,path_dem1,data1))// 0 means successfull conversation file to aster[][]
        {

                success= WriteDEM1(path_dem1,data1);
        }


return success;
}



