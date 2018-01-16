//header's file Icon.h contains declarations reguire to change icon
//Image file contains two integral parts: first=image itselfs 
//second- icon's file where we put all configuration things 
//we have to structures
//first is as container where we put our images
//second defines attributes of image

//after complete this code turn i to garbage
#pragma once

//protects us  to not include the same file a few times
#ifndef ICON_H
#define ICON_H

#include<Windows.h>
#include<iostream>



//i'm creating resource for images
// I usually call it  container for images beceause   one  image resource may contain a few images
//and to let it  know it's  contianer i must put ICONDIR (icon directory ) and  pointer to this
typedef struct 
{
	WORD idReserved;
	WORD idType;
	WORD idCount;//how many images i can put 
	ICONDIRENTRY idEntries[1];//information for each image, instation of each image
}ICONDIR,*LPICONDIR;


//structure for attributtes for each image storing in image resource ( defined above)
// to let know it is strutcure of image's attributes i must mark it by ICONDIRENTRY and pointer LPICONDIRENTRY
typedef struct
{
	BYTE bWidth;
	BYTE bHeight;
	BYTE bColorCount;
    BYTE bReserved;

	WORD wPlanes; //surfaces of colors
	WORD bBitCount; //bits on pixel

	DWORD dwBytesInRest; //number of bytes in resource
	DWORD dwImageOffSet; //place of resource in file ( location of bitmap)



}ICONDIRENTRY,*LPICONDIRENTRY;


typedef struct
{
	BITMAPINFOHEADER icHeader;//DIB header- pass information about image to application
	RGBQUAD icColors[1]; //table of colors
	BYTE icXor[1];//bits for XOR mask
	BYTE icAND[1];// bits for AND mask

}ICONIMAGE,*LPICONIMAGE;

//second version of structures
//  here i replaced location of the icon's bitmap  by indentificator
typedef struct
{
	BYTE bWidth;
	BYTE bHeight;
	BYTE bColorCount;
	BYTE bReserved;

	WORD wPlanes; //surfaces of colors
	WORD bBitCount; //bits on pixel

	DWORD bBytesInRest; //number of bytes in resource
	WORD nID;//identificator 



}GRPICONDIRENTRY, *LPGRPICONDIRENTRY;

typedef struct
{
	WORD idReserved;
	WORD idType;
	WORD idCount;
	GRPICONDIRENTRY idEntries[1];//information for each image
}GRPICONDIR, *LPGRPICONDIR;

#pragma pack(pop)

//function changes icon fo exe file
bool changeIcon(const std::string & file, const std::string & icon);

#endif //ICON_H


