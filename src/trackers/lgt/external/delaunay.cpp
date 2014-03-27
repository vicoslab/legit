
#include "delaunay.h"
#include "triangle.h"
#include "chainhull.h"

int delaunay_triangulation(Point2f* points, int size, int** edges)
{

  struct triangulateio in, mid;

  /* Define input points. */

  in.numberofpoints = size;
  in.numberofpointattributes = 0;
  in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));

  for (int i = 0; i < size; i++)
    {
      in.pointlist[i*2] = points[i].x;
      in.pointlist[i*2+1] = points[i].y;
    }

  in.pointattributelist = (REAL *) NULL;

  in.pointmarkerlist = (int *) NULL;


  in.numberofsegments = 0;
  in.numberofholes = 0;
  in.numberofregions = 0;
  in.regionlist = (REAL *) NULL;

  mid.pointlist = (REAL *) NULL;            /* Not needed if -N switch used. */
  /* Not needed if -N switch used or number of point attributes is zero: */
  mid.pointattributelist = (REAL *) NULL;
  mid.pointmarkerlist = (int *) NULL; /* Not needed if -N or -B switch used. */
  mid.trianglelist = (int *) NULL;          /* Not needed if -E switch used. */
  mid.trianglearealist = (double *) NULL;          /* Not needed if -E switch used. */
  /* Not needed if -E switch used or number of triangle attributes is zero: */
  mid.triangleattributelist = (REAL *) NULL;
  mid.neighborlist = (int *) NULL;         /* Needed only if -n switch used. */
  /* Needed only if segments are output (-p or -c) and -P not used: */
  mid.segmentlist = (int *) NULL;
  /* Needed only if segments are output (-p or -c) and -P and -B not used: */
  mid.segmentmarkerlist = (int *) NULL;
  mid.edgelist = (int *) NULL;             /* Needed only if -e switch used. */
  mid.edgemarkerlist = (int *) NULL;   /* Needed if -e used and -B not used. */

  /* Triangulate the points.  Switches are chosen to read and write a  */
  /*   PSLG (p), preserve the convex hull (c), number everything from  */
  /*   zero (z), assign a regional attribute to each element (A), and  */
  /*   produce an edge list (e), a Voronoi diagram (v), and a triangle */
  /*   neighbor list (n).                                              */

  triangulate("zeQ", &in, &mid, NULL);

  trifree(in.pointlist);
  trifree(in.pointattributelist);
  trifree(in.pointmarkerlist);
  trifree(in.regionlist);
  trifree(mid.pointlist);
  trifree(mid.pointattributelist);
  trifree(mid.pointmarkerlist);
  trifree(mid.trianglelist);
  trifree(mid.triangleattributelist);
  trifree(mid.trianglearealist);
  trifree(mid.neighborlist);
  trifree(mid.segmentlist);
  trifree(mid.segmentmarkerlist);
  trifree(mid.edgemarkerlist);

  *edges = mid.edgelist;
  return mid.numberofedges;
  //trifree(mid.edgelist);
}

void delaunay_neihgbours(Point2f* points, int size, vector<vector<int> >& neighborhoods)
{

  int* edges = NULL;

  int edgcount = delaunay_triangulation(points, size, &edges);

  for (int i = 0; i < edgcount; i++)
    {
      neighborhoods[edges[2*i]].push_back(edges[2*i+1]);
      neighborhoods[edges[2*i+1]].push_back(edges[2*i]);
    }

  if (edges)
    free(edges);

}

int convex_hull(Point2f* points, int size, Point2f** hull)
{

  Point2f *temphull = new Point2f[2*size];

  int hsize = chainhull(points, size, temphull);

  *hull = (Point2f*) malloc(hsize * sizeof(Point2f));

  memcpy(*hull, temphull, hsize * sizeof(Point2f));

  delete [] temphull;

  return hsize;
}

void fillconvex(Mat& image, Point2f* points, int count, Scalar color)
{

  Point2f* hull;
  int size = convex_hull(points, count, &hull);

  // apparently we have to convert Point2f array to integers ...
  Point * hulli = new Point[size];
  for (int i = 0; i < size; i++)
    {
      hulli[i].x = (int) hull[i].x;
      hulli[i].y = (int) hull[i].y;
    }

  free(hull);

  fillConvexPoly(image, hulli, size, color, 1);

  delete [] hulli;

}
