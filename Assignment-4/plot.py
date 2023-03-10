import networkx as nx
import matplotlib.pyplot as plt
import csv

# Load data from CSV file
with open('musae_git_edges.csv', 'r') as f:
    reader = csv.reader(f)
    edges = [tuple(map(int, row)) for row in reader]

# Construct directed graph from edges
print(len(edges))
G = nx.DiGraph()
G.add_edges_from(edges)

# Calculate in-degree of each node
in_degrees = dict(G.in_degree())
# print the max in-degree, min in-degree, and average in-degree
print(max(in_degrees.values()))
print(min(in_degrees.values()))
print(sum(in_degrees.values())/len(in_degrees.values()))


# Plot in-degree histogram
# plt.scatter(in_degrees.values(), bins=range(max(in_degrees.values()) + 2))
plt.scatter(in_degrees.keys(), in_degrees.values())
plt.xlabel('Node ID')
plt.ylabel('In-degree')
plt.title('In-degree Distribution')
plt.savefig('in_degree.png')
plt.show()

