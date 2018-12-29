
#include <iostream>
#include "lemon/list_graph.h"
#include "lemon/connectivity.h"
//#include "lemon/adaptors.h"

using namespace lemon;
using namespace std;

int main()
{
  typedef Undirector<ListDigraph> Graph;
  ListDigraph g;
  ListDigraph::Node u = g.addNode();
  ListDigraph::Node v = g.addNode();
  ListDigraph::Arc  a = g.addArc(u, v);
  cout << "Hello World! This is LEMON library here." << endl;
  cout << "We have a directed graph with " << countNodes(g) << " nodes "
       << "and " << countArcs(g) << " arc." << endl;

  Graph graph(g);
  Graph::NodeMap<int> comps(graph);
  connectedComponents( graph, comps );
  for ( Graph::NodeMap<int>::MapIt it(comps); it!=INVALID; ++it )
  {
    std::cout << graph.id( it ) << " <-> " << *it << std::endl;
  }

  return 0;
}




