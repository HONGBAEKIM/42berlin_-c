# CPP09

ex00
Reason why choose map container

1.    Ordered Associative Container: std::map is an ordered associative container. It maintains a sorted order of its elements based on the keys. This means you can easily iterate through the elements in a sorted order, which can be useful in various scenarios.

2.    Fast Lookup: std::map provides efficient key-based lookups (logarithmic time complexity). If you need to quickly search for elements based on their keys, std::map can be a good choice.

3.    No Duplicate Keys: std::map enforces unique keys. If you want to ensure that keys are unique in your container, std::map will automatically handle that for you.

4.    Balanced Tree Structure: Internally, std::map typically uses a balanced binary search tree (usually a Red-Black tree). This balanced structure ensures good performance characteristics even for large datasets.

5.    Insertion and Deletion: std::map supports efficient insertion and deletion of elements based on their keys.
