#include "Vision.h"

int main()
{
	Vision* vision = Vision::GetInstance();
	usleep(10000);
	Image* image = vision->GetFrame();
	printf("height %d, width %d, pixels %d\n", image->m_Height, image->m_Width, image->m_NumberOfPixels);
	for(int i = 0; i < image->m_Height; i++)
	{
		for(int j = 0; j < image->m_Width; j++)
			printf("%c ", image->m_ImageData[i*image->m_Height+j]);
		printf("\n");
	}
	return 0;
}
