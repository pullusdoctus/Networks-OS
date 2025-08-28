# Determine the degree, diameter and the bisection bandwidth of the network below:

![[topology.png]]

## Degree

Since neighbor count varies greatly from node to node, a concrete answer cannot be given. Instead, averaging the neighbor counts we get a degree of $\frac{10}{3}$.

## Diameter

The maximum, shortest distance found in this network is between nodes 'D' and 'F'. Assuming each edge has an equal weight of 1, the diameter is 4.

## Bisection Bandwith

The minimum amount of wires that can be cut to divide the network in half is by cutting across wires AB, AC, and EC. This means that this network's bisection bandwith is $3r$, where $r$ is its link bandwith.

# Draw a hybrid topology with a mesh backbone connecting two ring backbones. Each ring backbone connects three star networks.

![[hybrid_topology.svg]]