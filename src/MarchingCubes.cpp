#include "include/MarchingCubes.h"

void MarchingCubes::drawMarchingCubes()
{
  float red = m_particleproperties->getRed();
  float green = m_particleproperties->getGreen();
  float blue = m_particleproperties->getBlue();

  std::vector<Vec3> triangleVerticies;

  int render3dwidth=m_render3dGrid.size();
  int render3dheight=m_render3dGrid[0].size();
  int render3ddepth=m_render3dGrid[0][0].size();
  float isolevel=m_renderthreshold;

  //#pragma omp parallel for
  for(int w=0; w<render3dwidth-1; ++w)
  {
    for(int h=0; h<render3dheight-1; ++h)
    {
      for(int d=0; d<render3ddepth-1; ++d)
      {
        float gridvalue[8];

        gridvalue[0]=m_render3dGrid[w][h][d];     //0
        gridvalue[1]=m_render3dGrid[w+1][h][d];   //1
        gridvalue[2]=m_render3dGrid[w+1][h][d+1];   //2
        gridvalue[3]=m_render3dGrid[w][h][d+1];     //3

        gridvalue[4]=m_render3dGrid[w][h+1][d];     //4
        gridvalue[5]=m_render3dGrid[w+1][h+1][d];   //5
        gridvalue[6]=m_render3dGrid[w+1][h+1][d+1];     //6
        gridvalue[7]=m_render3dGrid[w][h+1][d+1];       //7

        float rendersquare=m_squaresize/m_renderresolution;

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
                vertexInterp(gridposition[0],gridposition[1],gridvalue[0],gridvalue[1]);
          if (edgeTable[cubeindex] & 2)
            vertlist[1] =
                vertexInterp(gridposition[1],gridposition[2],gridvalue[1],gridvalue[2]);
          if (edgeTable[cubeindex] & 4)
            vertlist[2] =
                vertexInterp(gridposition[2],gridposition[3],gridvalue[2],gridvalue[3]);
          if (edgeTable[cubeindex] & 8)
            vertlist[3] =
                vertexInterp(gridposition[3],gridposition[0],gridvalue[3],gridvalue[0]);
          if (edgeTable[cubeindex] & 16)
            vertlist[4] =
                vertexInterp(gridposition[4],gridposition[5],gridvalue[4],gridvalue[5]);
          if (edgeTable[cubeindex] & 32)
            vertlist[5] =
                vertexInterp(gridposition[5],gridposition[6],gridvalue[5],gridvalue[6]);
          if (edgeTable[cubeindex] & 64)
            vertlist[6] =
                vertexInterp(gridposition[6],gridposition[7],gridvalue[6],gridvalue[7]);
          if (edgeTable[cubeindex] & 128)
            vertlist[7] =
                vertexInterp(gridposition[7],gridposition[4],gridvalue[7],gridvalue[4]);
          if (edgeTable[cubeindex] & 256)
            vertlist[8] =
                vertexInterp(gridposition[0],gridposition[4],gridvalue[0],gridvalue[4]);
          if (edgeTable[cubeindex] & 512)
            vertlist[9] =
                vertexInterp(gridposition[1],gridposition[5],gridvalue[1],gridvalue[5]);
          if (edgeTable[cubeindex] & 1024)
            vertlist[10] =
                vertexInterp(gridposition[2],gridposition[6],gridvalue[2],gridvalue[6]);
          if (edgeTable[cubeindex] & 2048)
            vertlist[11] =
                vertexInterp(gridposition[3],gridposition[7],gridvalue[3],gridvalue[7]);

          for (int i=0;triTable[cubeindex][i]!=-1;i+=3)
          {
            Vec3 vectorA = (vertlist[triTable[cubeindex][i  ]] - vertlist[triTable[cubeindex][i+1]]) ;
            Vec3 vectorB = (vertlist[triTable[cubeindex][i  ]] - vertlist[triTable[cubeindex][i+2]]) ;
            Vec3 normal = vectorB.cross(vectorA);
            normal.normalize();
            triangleVerticies.push_back(normal);
            triangleVerticies.push_back(vertlist[triTable[cubeindex][i  ]]);
            triangleVerticies.push_back(vertlist[triTable[cubeindex][i+1]]);
            triangleVerticies.push_back(vertlist[triTable[cubeindex][i+2]]);
          }
        }
      }
    }
  }

  // #pragma omp parallel for
  for(int i = 0; i < triangleVerticies.size() ; i+=4)
  {
    glBegin(GL_TRIANGLES);
    glColor3f(red,green,blue);
    glNormal3f(triangleVerticies[i][0],triangleVerticies[i][1],triangleVerticies[i][2]);
    glVertex3f(triangleVerticies[i+1][0],triangleVerticies[i+1][1],triangleVerticies[i+1][2]);
    glVertex3f(triangleVerticies[i+2][0],triangleVerticies[i+2][1],triangleVerticies[i+2][2]);
    glVertex3f(triangleVerticies[i+3][0],triangleVerticies[i+3][1],triangleVerticies[i+3][2]);
    glEnd();
  }
}

Vec3 MarchingCubes::vertexInterp(Vec3 p1, Vec3 p2, float valp1, float valp2)
{

  float isolevel = m_renderthreshold;

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

void MarchingCubes::render3dGrid(std::vector<Particle> _particles, int _lastTakenParticle, float _interactionradius)
{
  std::vector<std::vector<std::vector<float>>> m_render3dGrid;
  m_render3dGrid.clear();
  m_render3dGrid.resize(m_renderwidth+1);
  for(auto& i : m_render3dGrid)
  {
    i.resize(m_renderheight+1);
    for(auto& j : i)
    {
      j.resize(m_renderwidth+1,0.0f);
    }
  }

  float rendersquare=m_squaresize/m_renderresolution;

  for(int i=0; i<_lastTakenParticle+1; ++i)
  {
    if(_particles[i].isAlive()&&(_particles[i].getProperties()==m_particleproperties))
    {
      Vec3 heightwidthdepth = getGridXYZ(_particles[i].getGridPosition())*m_renderresolution; // 3Dify this

      for(int x = -2*m_renderresolution; x<=4*m_renderresolution; ++x)
      {
        for(int y = -2*m_renderresolution; y<=4*m_renderresolution ; ++y)
        {
          for(int z = -2*m_renderresolution; z<=4*m_renderresolution ; ++z)
          {
            int currentcolumn=heightwidthdepth[0]+x;
            int currentrow=heightwidthdepth[1]+y;
            int currentdepth=heightwidthdepth[2]+z;

            if(currentcolumn<m_renderwidth && currentcolumn>0 &&
               currentrow<m_renderheight && currentrow>0 &&
               currentdepth<m_renderwidth && currentdepth>0)
            {
              float currentx = rendersquare*(float)currentcolumn - m_halfwidth;
              float currenty = rendersquare*(float)currentrow - m_halfheight;
              float currentz = rendersquare*(float)currentdepth - 2 - m_halfwidth;

              float metaballx = currentx-_particles[i].getPosition()[0];
              float metabally = currenty-_particles[i].getPosition()[1];
              float metaballz = currentz-_particles[i].getPosition()[2];

              float metaballfloat = (_interactionradius*_interactionradius)/(metaballx*metaballx + metabally*metabally + metaballz*metaballz);
              m_render3dGrid[currentcolumn][currentrow][currentdepth]+=metaballfloat;
            }
          }
        }
      }
    }
  }
}

Vec3 MarchingCubes::getGridXYZ(int k) // CHECK THIS
{
    int z = floor(k/(m_gridwidth*m_gridheight));
    int y = floor((k - z*m_gridwidth*m_gridheight)/m_gridwidth);
    int x = k - y*m_gridwidth - z*m_gridwidth*m_gridheight;

    return Vec3(x,y,z);
}
