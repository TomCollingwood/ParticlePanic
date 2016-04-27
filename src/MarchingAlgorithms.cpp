#include "include/MarchingAlgorithms.h"

MarchingAlgorithms::MarchingAlgorithms()
{

}

void MarchingAlgorithms::drawMarchingCubes(std::vector<std::vector<std::vector<float>>> renderGrid, ParticleProperties p)
{
  float red = p.getRed();
  float green = p.getGreen();
  float blue = p.getBlue();

  int render3dheight=renderGrid.size()-1;
  int render3dwidth=renderGrid[0].size()-1;
  int render3ddepth=renderGrid[0][0].size()-1;

  float isolevel=m_mainrender3dthreshold; //can set at initialization

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

        float rendersquare=squaresize/render3dresolution;

        float wWorld = - halfwidth + w * rendersquare;
        float hWorld = - halfheight + h * rendersquare ;
        float dWorld = - 2 - halfwidth + d * rendersquare;

        std::vector<Vec3> gridposition;
        gridposition.push_back(Vec3(wWorld,             hWorld,             dWorld));               //0
        gridposition.push_back(Vec3(wWorld+rendersquare,hWorld,             dWorld));               //1
        gridposition.push_back(Vec3(wWorld+rendersquare,hWorld,             dWorld+rendersquare));  //2
        gridposition.push_back(Vec3(wWorld,             hWorld,             dWorld+rendersquare));  //3

        gridposition.push_back(Vec3(wWorld,             hWorld+rendersquare,dWorld));               //4
        gridposition.push_back(Vec3(wWorld+rendersquare,hWorld+rendersquare,dWorld));               //5
        gridposition.push_back(Vec3(wWorld+rendersquare,hWorld+rendersquare,dWorld+rendersquare));               //6
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
            m_snapshotTriangles[w][h][d].push_back(Vec3(red,green,blue));
            m_snapshotTriangles[w][h][d].push_back(normal);
            m_snapshotTriangles[w][h][d].push_back(vertlist[triTable[cubeindex][i  ]]);
            m_snapshotTriangles[w][h][d].push_back(normal);
            m_snapshotTriangles[w][h][d].push_back(vertlist[triTable[cubeindex][i+1]]);
            m_snapshotTriangles[w][h][d].push_back(normal);
            m_snapshotTriangles[w][h][d].push_back(vertlist[triTable[cubeindex][i+2]]);
          }
        }
      }
    }
  }


  if(m_snapshotMode==2)
  {
    /*
    std::vector<std::vector<std::vector<std::vector<Vec3>>>> temporarynormals = m_snapshotTriangles;

    // CALCULATE NORMALS
    for(int w=0; w<render3dwidth; ++w)
    {
      for(int h=0; h<render3dheight; ++h)
      {
        for(int d=0; d<render3ddepth; ++d)
        {
          for(int k=0; k<m_snapshotTriangles[w][h][d].size(); k+=7)
          {
            for(int j=1; j<7; j+=2)
            {
              for(int wa=-1; wa<2; ++wa)
              {
                for(int ha=-1; ha<2; ++ha)
                {
                  for(int da=-1; da<2; ++da)
                  {
                      for(int p=0; p<m_snapshotTriangles[w+wa][h+ha][d+da].size(); p+=7)
                      {
                        for(int l=1; l<7; l+=2)
                        {
                          //std::cout<<"HERE3"<<std::endl;
                          if(w+wa<render3dwidth && w+wa>=0 &&
                             h+ha<render3dheight && h+ha>=0 &&
                             d+da<render3ddepth && d+da>=0 &&
                             !(da==0 && wa==0 && da==0 && (k+j==p+l)))
                          {
                            //std::cout<<"HERE2"<<std::endl;
                            if(m_snapshotTriangles[w+wa][h+ha][d+da][p+l+1]==m_snapshotTriangles[w][h][d][k+j+1])
                            {
                              //std::cout<<"HERE"<<std::endl;
                              temporarynormals[w][h][d][k+j]+=m_snapshotTriangles[w+wa][h+ha][d+da][p+l];
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

    m_snapshotTriangles=temporarynormals;


    // NORMALIZE NORMALS
    for(int w=0; w<render3dwidth-1; ++w)
    {
      for(int h=0; h<render3dheight-1; ++h)
      {
        for(int d=0; d<render3ddepth-1; ++d)
        {
          for(int k=0; k<m_snapshotTriangles[w][h][d].size(); k+=7)
          {
            for(int j=1; j<7; j+=2)
            {
              m_snapshotTriangles[w][h][d][k+j].normalize();
            }
          }
        }
      }
    }
    // */
  }


}

