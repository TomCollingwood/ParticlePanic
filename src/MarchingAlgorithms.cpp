///
///  @file    MarchingAlgorithms.cpp
///  @brief   Contains the marching cube / square triangle vectors and algorithms to draw and create them
///  @author  Paul Bourke & Thomas Collingwood

#include "include/MarchingAlgorithms.h"

/// The following section is modified from :-
/// Paul Bourke (1994). Polygonising a scalar field [online]. [Accessed 2016].
/// Available from: <http://paulbourke.net/geometry/polygonise/>.
void MarchingAlgorithms::calculateMarchingCubes(std::vector<std::vector<std::vector<float>>> renderGrid, ParticleProperties p)
{
  float red = p.getRed();
  float green = p.getGreen();
  float blue = p.getBlue();

  int render3dwidth=renderGrid.size()-1;
  int render3dheight=renderGrid[0].size()-1;
  int render3ddepth=renderGrid[0][0].size()-1;

  float isolevel=m_render3dThreshold; //can set at initialization

  //#pragma omp parallel for
  for(int w=0; w<render3dwidth; ++w)
  {
    for(int h=0; h<render3dheight; ++h)
    {
      for(int d=0; d<render3ddepth; ++d)
      {
        float gridvalue[8];

        gridvalue[0]=renderGrid[w][h][d];     //0
        gridvalue[1]=renderGrid[w+1][h][d];   //1
        gridvalue[2]=renderGrid[w+1][h][d+1];   //2
        gridvalue[3]=renderGrid[w][h][d+1];     //3

        gridvalue[4]=renderGrid[w][h+1][d];     //4
        gridvalue[5]=renderGrid[w+1][h+1][d];   //5
        gridvalue[6]=renderGrid[w+1][h+1][d+1];     //6
        gridvalue[7]=renderGrid[w][h+1][d+1];       //7

        float rendersquare=m_squaresize/m_render3dresolution;

        float wWorld = - m_halfwidth + w * rendersquare;
        float hWorld = - m_halfheight + h * rendersquare ;
        float dWorld = - 2 - m_halfwidth + d * rendersquare;

        std::vector<Vec3> gridposition;
        gridposition.push_back(Vec3(wWorld,             hWorld,             dWorld));               //0
        gridposition.push_back(Vec3(wWorld+rendersquare,hWorld,             dWorld));               //1
        gridposition.push_back(Vec3(wWorld+rendersquare,hWorld,             dWorld+rendersquare));  //2
        gridposition.push_back(Vec3(wWorld,             hWorld,             dWorld+rendersquare));  //3

        gridposition.push_back(Vec3(wWorld,             hWorld+rendersquare,dWorld));               //4
        gridposition.push_back(Vec3(wWorld+rendersquare,hWorld+rendersquare,dWorld));               //5
        gridposition.push_back(Vec3(wWorld+rendersquare,hWorld+rendersquare,dWorld+rendersquare));  //6
        gridposition.push_back(Vec3(wWorld,             hWorld+rendersquare,dWorld+rendersquare));  //7

        Vec3 vertlist[12];

        int cubeindex = 0;
        if (gridvalue[0] < isolevel) cubeindex |= 1;
        if (gridvalue[1] < isolevel) cubeindex |= 2;
        if (gridvalue[2] < isolevel) cubeindex |= 4;
        if (gridvalue[3] < isolevel) cubeindex |= 8;
        if (gridvalue[4] < isolevel) cubeindex |= 16;
        if (gridvalue[5] < isolevel) cubeindex |= 32;
        if (gridvalue[6] < isolevel) cubeindex |= 64;
        if (gridvalue[7] < isolevel) cubeindex |= 128;

        if(edgeTable[cubeindex]!=0)
        {
          // Find the vertices where the surface intersects the cube
          if (edgeTable[cubeindex] & 1)
            vertlist[0] =
                VertexInterp(gridposition[0],gridposition[1],gridvalue[0],gridvalue[1]);
          if (edgeTable[cubeindex] & 2)
            vertlist[1] =
                VertexInterp(gridposition[1],gridposition[2],gridvalue[1],gridvalue[2]);
          if (edgeTable[cubeindex] & 4)
            vertlist[2] =
                VertexInterp(gridposition[2],gridposition[3],gridvalue[2],gridvalue[3]);
          if (edgeTable[cubeindex] & 8)
            vertlist[3] =
                VertexInterp(gridposition[3],gridposition[0],gridvalue[3],gridvalue[0]);
          if (edgeTable[cubeindex] & 16)
            vertlist[4] =
                VertexInterp(gridposition[4],gridposition[5],gridvalue[4],gridvalue[5]);
          if (edgeTable[cubeindex] & 32)
            vertlist[5] =
                VertexInterp(gridposition[5],gridposition[6],gridvalue[5],gridvalue[6]);
          if (edgeTable[cubeindex] & 64)
            vertlist[6] =
                VertexInterp(gridposition[6],gridposition[7],gridvalue[6],gridvalue[7]);
          if (edgeTable[cubeindex] & 128)
            vertlist[7] =
                VertexInterp(gridposition[7],gridposition[4],gridvalue[7],gridvalue[4]);
          if (edgeTable[cubeindex] & 256)
            vertlist[8] =
                VertexInterp(gridposition[0],gridposition[4],gridvalue[0],gridvalue[4]);
          if (edgeTable[cubeindex] & 512)
            vertlist[9] =
                VertexInterp(gridposition[1],gridposition[5],gridvalue[1],gridvalue[5]);
          if (edgeTable[cubeindex] & 1024)
            vertlist[10] =
                VertexInterp(gridposition[2],gridposition[6],gridvalue[2],gridvalue[6]);
          if (edgeTable[cubeindex] & 2048)
            vertlist[11] =
                VertexInterp(gridposition[3],gridposition[7],gridvalue[3],gridvalue[7]);

          for (int i=0;triTable[cubeindex][i]!=-1;i+=3)
          {
            Vec3 vectorA = (vertlist[triTable[cubeindex][i  ]] - vertlist[triTable[cubeindex][i+1]]) ;
            Vec3 vectorB = (vertlist[triTable[cubeindex][i  ]] - vertlist[triTable[cubeindex][i+2]]) ;
            Vec3 normal = vectorB.cross(vectorA);
            if(!m_snapshotMode) normal.normalize();
            if(m_snapshotMode==2)
            {
              m_snapshot3DTriangles[w][h][d].push_back(Vec3(red,green,blue));
              m_snapshot3DTriangles[w][h][d].push_back(normal);
              m_snapshot3DTriangles[w][h][d].push_back(vertlist[triTable[cubeindex][i  ]]);
              m_snapshot3DTriangles[w][h][d].push_back(normal);
              m_snapshot3DTriangles[w][h][d].push_back(vertlist[triTable[cubeindex][i+1]]);
              m_snapshot3DTriangles[w][h][d].push_back(normal);
              m_snapshot3DTriangles[w][h][d].push_back(vertlist[triTable[cubeindex][i+2]]);
            }
            else
            {
              m_realtime3DTriangles.push_back(Vec3(red,green,blue));
              m_realtime3DTriangles.push_back(normal);
              m_realtime3DTriangles.push_back(vertlist[triTable[cubeindex][i  ]]);
              m_realtime3DTriangles.push_back(vertlist[triTable[cubeindex][i+1]]);
              m_realtime3DTriangles.push_back(vertlist[triTable[cubeindex][i+2]]);
            }
          }
        }
      }
    }
  }


  if(m_snapshotMode==2)
  {

    std::vector<std::vector<std::vector<std::vector<Vec3>>>> temporarynormals = m_snapshot3DTriangles;

    // CALCULATE VERTEX NORMALS
    for(int w=0; w<render3dwidth; ++w)
    {
      for(int h=0; h<render3dheight; ++h)
      {
        for(int d=0; d<render3ddepth; ++d)
        {
          for(int k=0; k<m_snapshot3DTriangles[w][h][d].size(); k+=7)
          {
            for(int j=1; j<7; j+=2)
            {
              for(int wa=-1; wa<2; ++wa)
              {
                for(int ha=-1; ha<2; ++ha)
                {
                  for(int da=-1; da<2; ++da)
                  {
                    if(w+wa<render3dwidth && w+wa>=0 &&
                       h+ha<render3dheight && h+ha>=0 &&
                       d+da<render3ddepth && d+da>=0)
                    {
                      for(int p=0; p<m_snapshot3DTriangles[w+wa][h+ha][d+da].size(); p+=7)
                      {
                        for(int l=1; l<7; l+=2)
                        {
                          //std::cout<<"HERE3"<<std::endl;
                          if(!(da==0 && wa==0 && da==0 && (k+j==p+l)))
                          {
                            //std::cout<<"HERE2"<<std::endl;
                            if(m_snapshot3DTriangles[w+wa][h+ha][d+da][p+l+1]==m_snapshot3DTriangles[w][h][d][k+j+1])
                            {
                              //std::cout<<"HERE"<<std::endl;
                              temporarynormals[w][h][d][k+j]+=m_snapshot3DTriangles[w+wa][h+ha][d+da][p+l];
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }

    m_snapshot3DTriangles=temporarynormals;


    // NORMALIZE NORMALS
    for(int w=0; w<render3dwidth-1; ++w)
    {
      for(int h=0; h<render3dheight-1; ++h)
      {
        for(int d=0; d<render3ddepth-1; ++d)
        {
          for(int k=0; k<m_snapshot3DTriangles[w][h][d].size(); k+=7)
          {
            for(int j=1; j<7; j+=2)
            {
              m_snapshot3DTriangles[w][h][d][k+j].normalize();
            }
          }
        }
      }
    }
    // */
  }
}
/// end of Citation

void MarchingAlgorithms::calculateMarchingSquares(const std::vector<std::vector<float>> renderGrid,
                                                  const ParticleProperties p,
                                                  const bool inner)
{
  float red = p.getRed();
  float green = p.getGreen();
  float blue = p.getBlue();

  float renderthreshold = m_render2dThreshold;

  int renderheight = renderGrid.size()-1;
  int renderwidth = renderGrid[0].size()-1;

  if(inner)
  {
    renderthreshold=0.7f*renderthreshold;
    red+=0.4;
    green+=0.4;
    blue+=0.4;
  }

  float rendersquare=m_squaresize/m_renderresolution;

  for(int currentrow=0; currentrow<renderheight; ++currentrow)
  {
    for(int currentcolumn=0; currentcolumn<renderwidth; ++currentcolumn)
    {

      //1---5---2
      //|       |
      //8       6
      //|       |
      //3---7---4


      std::vector<bool> boolpoints;
      boolpoints.push_back(renderGrid[currentrow][currentcolumn]>renderthreshold);
      boolpoints.push_back(renderGrid[currentrow][currentcolumn+1]>renderthreshold);
      boolpoints.push_back(renderGrid[currentrow+1][currentcolumn]>renderthreshold);
      boolpoints.push_back(renderGrid[currentrow+1][currentcolumn+1]>renderthreshold);

      bool empty=false;

      if(!boolpoints[0]&&!boolpoints[1]&&!boolpoints[2]&&!boolpoints[3])
      {
        empty=true;
      }

      if(!empty)
      {
        float p1x = (m_squaresize/m_renderresolution)*(float)currentcolumn - m_halfwidth;
        float p1y = (m_squaresize/m_renderresolution)*(float)currentrow - m_halfheight;

        float p2x = (m_squaresize/m_renderresolution)*((float)currentcolumn+1.0f) - m_halfwidth;
        float p2y = p1y;

        float p3x = p1x;
        float p3y = (m_squaresize/m_renderresolution)*((float)currentrow+1.0f) - m_halfheight;

        float p4x = p2x;
        float p4y = p3y;


        //float p5x = (p1x+p2x)/2.0f;
        float p5y = p1y;

        float p6x = p2x;
        //float p6y = (p2y+p4y)/2.0f;

        //float p7x = p5x;
        float p7y = p3y;

        float p8x = p1x;
        //float p8y = p6y;


        if(boolpoints[0]&&boolpoints[1]&&boolpoints[2]&&boolpoints[3]) //1111 TICK WAS A QUAD
        {

          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p1x,p1y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p2x,p2y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p4x,p4y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p3x,p3y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p1x,p1y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p4x,p4y,-2.0f));

        }
        else if(!boolpoints[0]&&!boolpoints[1]&&!boolpoints[2]&&boolpoints[3]) //0001 TICK
        {
          float p6y=p2y+rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn+1])/(renderGrid[currentrow+1][currentcolumn+1]-renderGrid[currentrow][currentcolumn+1]));
          float p7x=p3x+rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn])/(renderGrid[currentrow+1][currentcolumn+1]-renderGrid[currentrow+1][currentcolumn]));


          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p6x,p6y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p7x,p7y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p4x,p4y,-2.0f));

        }
        else if(!boolpoints[0]&&!boolpoints[1]&&boolpoints[2]&&!boolpoints[3]) //0010 TICK
        {
          float p8y=p1y+rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn])/(renderGrid[currentrow+1][currentcolumn]-renderGrid[currentrow][currentcolumn]));
          float p7x=p4x-rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn+1])/(renderGrid[currentrow+1][currentcolumn]-renderGrid[currentrow+1][currentcolumn+1]));


          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p8x,p8y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p7x,p7y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p3x,p3y,-2.0f));

        }
        else if(!boolpoints[0]&&!boolpoints[1]&&boolpoints[2]&&boolpoints[3]) //0011 TICk QUAD
        {
          float p8y=p1y+rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn])/(renderGrid[currentrow+1][currentcolumn]-renderGrid[currentrow][currentcolumn]));
          float p6y=p2y+rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn+1])/(renderGrid[currentrow+1][currentcolumn+1]-renderGrid[currentrow][currentcolumn+1]));

          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p8x,p8y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p6x,p6y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p4x,p4y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p3x,p3y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p8x,p8y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p4x,p4y,-2.0f));

        }
        else if(!boolpoints[0]&&boolpoints[1]&&!boolpoints[2]&&!boolpoints[3]) //0100 TICK
        {
          float p6y=p4y-rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn+1])/(renderGrid[currentrow][currentcolumn+1]-renderGrid[currentrow+1][currentcolumn+1]));
          float p5x=p1x+rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn])/(renderGrid[currentrow][currentcolumn+1]-renderGrid[currentrow][currentcolumn]));


          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p5x,p5y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p2x,p2y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p6x,p6y,-2.0f));

        }
        else if(!boolpoints[0]&&boolpoints[1]&&!boolpoints[2]&&boolpoints[3]) //0101 TICK QUAD
        {
          float p5x=p1x+rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn])/(renderGrid[currentrow][currentcolumn+1]-renderGrid[currentrow][currentcolumn]));
          float p7x=p3x+rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn])/(renderGrid[currentrow+1][currentcolumn+1]-renderGrid[currentrow+1][currentcolumn]));

          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p5x,p5y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p2x,p2y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p4x,p4y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p7x,p7y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p5x,p5y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p4x,p4y,-2.0f));

        }
        else if(!boolpoints[0]&&boolpoints[1]&&boolpoints[2]&&!boolpoints[3]) //0110 COULD CHANGE TO SEE
        {
          float p5x=p1x+(p2x-p1x)*((renderthreshold-renderGrid[currentrow][currentcolumn])/(renderGrid[currentrow][currentcolumn+1]-renderGrid[currentrow][currentcolumn]));
          float p6y=p4y+(p2y-p4y)*((renderthreshold-renderGrid[currentrow+1][currentcolumn+1])/(renderGrid[currentrow][currentcolumn+1]-renderGrid[currentrow+1][currentcolumn+1]));
          float p7x=p4x+(p3x-p4x)*((renderthreshold-renderGrid[currentrow+1][currentcolumn+1])/(renderGrid[currentrow+1][currentcolumn]-renderGrid[currentrow+1][currentcolumn+1]));
          float p8y=p1y+(p3y-p1y)*((renderthreshold-renderGrid[currentrow][currentcolumn])/(renderGrid[currentrow+1][currentcolumn]-renderGrid[currentrow][currentcolumn]));


          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p5x,p5y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p2x,p2y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p6x,p6y,-2.0f));

          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p5x,p5y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p6x,p6y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p8x,p8y,-2.0f));

          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p8x,p8y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p6x,p6y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p7x,p7y,-2.0f));

          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p8x,p8y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p7x,p7y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p3x,p3y,-2.0f));


        }
        else if(!boolpoints[0]&&boolpoints[1]&&boolpoints[2]&&boolpoints[3]) //0111 TICK
        {
          float p5x=p1x+rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn])/(renderGrid[currentrow][currentcolumn+1]-renderGrid[currentrow][currentcolumn]));
          float p8y=p1y+rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn])/(renderGrid[currentrow+1][currentcolumn]-renderGrid[currentrow][currentcolumn]));


          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p5x,p5y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p2x,p2y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p4x,p4y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p5x,p5y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p4x,p4y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p8x,p8y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p8x,p8y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p4x,p4y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p3x,p3y,-2.0f));
        }
        else if(boolpoints[0]&&!boolpoints[1]&&!boolpoints[2]&&!boolpoints[3]) //1000 TICK
        {
          float p5x=p2x-rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn+1])/(renderGrid[currentrow][currentcolumn]-renderGrid[currentrow][currentcolumn+1]));
          float p8y=p3y-rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn])/(renderGrid[currentrow][currentcolumn]-renderGrid[currentrow+1][currentcolumn]));


          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p1x,p1y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p5x,p5y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p8x,p8y,-2.0f));

        }
        else if(boolpoints[0]&&!boolpoints[1]&&!boolpoints[2]&&boolpoints[3]) //1001 COULD CHANGE TO SEE
        {
          float p5x=p2x+(p1x-p2x)*((renderthreshold-renderGrid[currentrow][currentcolumn+1])/(renderGrid[currentrow][currentcolumn]-renderGrid[currentrow][currentcolumn+1]));
          float p6y=p2y+(p4y-p2y)*((renderthreshold-renderGrid[currentrow][currentcolumn+1])/(renderGrid[currentrow+1][currentcolumn+1]-renderGrid[currentrow][currentcolumn+1]));
          float p7x=p3x+(p4x-p3x)*((renderthreshold-renderGrid[currentrow+1][currentcolumn])/(renderGrid[currentrow+1][currentcolumn+1]-renderGrid[currentrow+1][currentcolumn]));
          float p8y=p3y+(p1y-p3y)*((renderthreshold-renderGrid[currentrow+1][currentcolumn])/(renderGrid[currentrow][currentcolumn]-renderGrid[currentrow+1][currentcolumn]));



          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p1x,p1y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p5x,p5y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p8x,p8y,-2.0f));

          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p8x,p8y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p5x,p5y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p6x,p6y,-2.0f));

          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p8x,p8y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p6x,p6y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p7x,p7y,-2.0f));

          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p6x,p6y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p4x,p4y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p7x,p7y,-2.0f));


        }
        else if(boolpoints[0]&&!boolpoints[1]&&boolpoints[2]&&!boolpoints[3]) //1010 TICK QUAD
        {
          float p5x=p2x-rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn+1])/(renderGrid[currentrow][currentcolumn]-renderGrid[currentrow][currentcolumn+1]));
          float p7x=p4x-rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn+1])/(renderGrid[currentrow+1][currentcolumn]-renderGrid[currentrow+1][currentcolumn+1]));


          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p1x,p1y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p5x,p5y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p7x,p7y,-2.0f));

          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p3x,p3y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p1x,p1y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p7x,p7y,-2.0f));

        }
        else if(boolpoints[0]&&!boolpoints[1]&&boolpoints[2]&&boolpoints[3]) //1011 TICK
        {
          float p5x=p2x-rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn+1])/(renderGrid[currentrow][currentcolumn]-renderGrid[currentrow][currentcolumn+1]));
          float p6y=p2y+rendersquare*((renderthreshold-renderGrid[currentrow][currentcolumn+1])/(renderGrid[currentrow+1][currentcolumn+1]-renderGrid[currentrow][currentcolumn+1]));


          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p1x,p1y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p5x,p5y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p3x,p3y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p3x,p3y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p5x,p5y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p6x,p6y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p3x,p3y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p6x,p6y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p4x,p4y,-2.0f));

        }
        else if(boolpoints[0]&&boolpoints[1]&&!boolpoints[2]&&!boolpoints[3]) //1100 TICK QUAD
        {
          float p6y=p4y-rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn+1])/(renderGrid[currentrow][currentcolumn+1]-renderGrid[currentrow+1][currentcolumn+1]));
          float p8y=p3y-rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn])/(renderGrid[currentrow][currentcolumn]-renderGrid[currentrow+1][currentcolumn]));


          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p1x,p1y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p2x,p2y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p6x,p6y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p8x,p8y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p1x,p1y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p6x,p6y,-2.0f));

        }
        else if(boolpoints[0]&&boolpoints[1]&&!boolpoints[2]&&boolpoints[3]) //1101 TICK
        {
          float p7x=p3x+rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn])/(renderGrid[currentrow+1][currentcolumn+1]-renderGrid[currentrow+1][currentcolumn]));
          float p8y=p3y-rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn])/(renderGrid[currentrow][currentcolumn]-renderGrid[currentrow+1][currentcolumn]));


          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p1x,p1y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p2x,p2y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p8x,p8y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p2x,p2y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p7x,p7y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p8x,p8y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p2x,p2y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p7x,p7y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p4x,p4y,-2.0f));

        }
        else if(boolpoints[0]&&boolpoints[1]&&boolpoints[2]&&!boolpoints[3]) //1110
        {
          float p7x=p4x-rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn+1])/(renderGrid[currentrow+1][currentcolumn]-renderGrid[currentrow+1][currentcolumn+1]));
          float p6y=p4y-rendersquare*((renderthreshold-renderGrid[currentrow+1][currentcolumn+1])/(renderGrid[currentrow][currentcolumn+1]-renderGrid[currentrow+1][currentcolumn+1]));


          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p1x,p1y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p2x,p2y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p6x,p6y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p1x,p1y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p6x,p6y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p7x,p7y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(red,green,blue));
          m_realtime2DTriangles.push_back(Vec3(p1x,p1y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p7x,p7y,-2.0f));
          m_realtime2DTriangles.push_back(Vec3(p3x,p3y,-2.0f));

        }
      }
    }
  }
}

void MarchingAlgorithms::draw3DSnapshot()
{
  glBegin(GL_TRIANGLES);
  for(auto& i : m_snapshot3DTriangles)
  {
    for(auto& j : i)
    {
      for(auto& k : j)
      {
        for(int l = 0; l<(int)k.size() ; l+=7)
        {
          glColor3f(k[l][0],k[l][1],k[l][2]);
          glNormal3f(k[l+1][0],k[l+1][1],k[l+1][2]);
          glVertex3f(k[l+2][0],k[l+2][1],k[l+2][2]);
          glNormal3f(k[l+3][0],k[l+3][1],k[l+3][2]);
          glVertex3f(k[l+4][0],k[l+4][1],k[l+4][2]);
          glNormal3f(k[l+5][0],k[l+5][1],k[l+5][2]);
          glVertex3f(k[l+6][0],k[l+6][1],k[l+6][2]);
        }
      }
    }
  }
  glEnd();
}

void MarchingAlgorithms::draw3DRealtime()
{
  glBegin(GL_TRIANGLES);
  for(int i =0; i<m_realtime3DTriangles.size(); i+=5)
  {
    glColor3f(m_realtime3DTriangles[i][0],m_realtime3DTriangles[i][1],m_realtime3DTriangles[i][2]);
    glNormal3f(m_realtime3DTriangles[i+1][0],m_realtime3DTriangles[i+1][1],m_realtime3DTriangles[i+1][2]);
    glVertex3f(m_realtime3DTriangles[i+2][0],m_realtime3DTriangles[i+2][1],m_realtime3DTriangles[i+2][2]);
    glVertex3f(m_realtime3DTriangles[i+3][0],m_realtime3DTriangles[i+3][1],m_realtime3DTriangles[i+3][2]);
    glVertex3f(m_realtime3DTriangles[i+4][0],m_realtime3DTriangles[i+4][1],m_realtime3DTriangles[i+4][2]);
  }
  glEnd();
  clearRealtime3DTriangles();
}

void MarchingAlgorithms::draw2DRealtime()
{
  glDisable(GL_LIGHTING);
  glBegin(GL_TRIANGLES);
  for(int i =0; i<(int)m_realtime2DTriangles.size(); i+=4)
  {
    glColor3f(m_realtime2DTriangles[i][0],m_realtime2DTriangles[i][1],m_realtime2DTriangles[i][2]);
    glVertex3f(m_realtime2DTriangles[i+1][0],m_realtime2DTriangles[i+1][1],m_realtime2DTriangles[i+1][2]);
    glVertex3f(m_realtime2DTriangles[i+2][0],m_realtime2DTriangles[i+2][1],m_realtime2DTriangles[i+2][2]);
    glVertex3f(m_realtime2DTriangles[i+3][0],m_realtime2DTriangles[i+3][1],m_realtime2DTriangles[i+3][2]);
  }
  glEnd();
  glEnable(GL_LIGHTING);
  clearRealtime2DTriangles();
}

/// The following section is modified from :-
/// Paul Bourke (1994). Polygonising a scalar field [online]. [Accessed 2016].
/// Available from: <http://paulbourke.net/geometry/polygonise/>.
Vec3 MarchingAlgorithms::VertexInterp(Vec3 p1, Vec3 p2, float valp1, float valp2)
{
  float isolevel = m_render3dThreshold;

  double mu;
  Vec3 p = Vec3();

  if (std::abs(isolevel-valp1) < 0.00001)
    return p1;
  if (std::abs(isolevel-valp2) < 0.00001)
    return p2 ;
  if (std::abs(valp1-valp2) < 0.00001)
    return p1 ;
  mu = (isolevel - valp1) / (valp2 - valp1);
  p[0] = p1[0] + mu * (p2[0] - p1[0]);
  p[1] = p1[1] + mu * (p2[1] - p1[1]);
  p[2] = p1[2] + mu * (p2[2] - p1[2]);

  return p;
}
/// end of Citation

void MarchingAlgorithms::clearRealtime2DTriangles()
{
  m_realtime2DTriangles.clear();
}

void MarchingAlgorithms::clearRealtime3DTriangles()
{
  m_realtime3DTriangles.clear();
}

void MarchingAlgorithms::clearSnapshot3DTriangles()
{
  m_snapshot3DTriangles.clear();

  int gridwidth=ceil((m_halfwidth*2)/m_squaresize);
  int gridheight=ceil((m_halfheight*2)/m_squaresize);
  int render3dwidth=gridwidth*m_render3dresolution;
  int render3dheight=gridheight*m_render3dresolution;

  m_snapshot3DTriangles.resize(render3dwidth);
  for(auto& i : m_snapshot3DTriangles)
  {
    i.resize(render3dheight);
    for(auto& j : i)
    {
      j.resize(render3dwidth);
    }
  }
}

int MarchingAlgorithms::getSnapshotMode()
{
  return m_snapshotMode;
}

void MarchingAlgorithms::setSnapshotMode(int _s)
{
  m_snapshotMode=_s;
}

void MarchingAlgorithms::increase2DResolution()
{
  ++m_renderresolution;
}

void MarchingAlgorithms::decrease2DResolution()
{
  if(m_renderresolution!=1)
    --m_renderresolution;
}

void MarchingAlgorithms::setSquareSize(float ss)
{
  m_squaresize=ss;
}

void MarchingAlgorithms::toggle3DResolution()
{
  if(!m_ishighres)
  {
    m_render3dresolution*=m_snapshotmultiplier;
    m_ishighres=true;
  }
  else
  {
    m_render3dresolution/=m_snapshotmultiplier;
    m_ishighres=false;
  }
}

