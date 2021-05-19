import sys
global vector_list
global clusters
global centroids
vector_list = []
clusters = []
centroids = []
def main(max_iter=200):
    argc = len(sys.argv)
    assert(argc == 3 or argc == 2 )
    if (argc == 3):
        max_iter = int (sys.argv[2])
    assert (max_iter >= 0)
    K = int (sys.argv[1])
    assert (K >= 0)
    vector_list = readfile()
    init_centroids(K)
    counter = 0
    isequal = True
    while (counter<max_iter and isequal):
        vector_to_cluster(K) #update all clusters with vectors
        isequal = update_centroids() 
        counter+=1
    for centroid in centroids:
        for i in range(len(centroid)):
            centroid[i] = "%.4f" % centroid[i]
        print(*centroid, sep = ", ")
            
    return centroids

def readfile():
    #f = open(filename, "r")
    for line in sys.stdin:
    #for line in f:
        line.rstrip("/n")
        vector = line.split(",")
        for i in range(len(vector)):
            number = vector[i]
            vector[i] = float(number)
        vector_list.append(vector)

def init_centroids(k):
    assert k<len(vector_list)
    for i in range(k):
        centroids.append(vector_list[i])
        clusters.append([])

def distance(v1,v2):
    res = 0
    for i in range(len(v1)):
        res += (v1[i]-v2[i])**2
    return res

def min_dist_centroid(v):
    minimum = distance(v, centroids[0])
    ind = 0
    for i in range(len(centroids)):
        dist = distance(v, centroids[i])
        if dist < minimum:
            minimum = dist
            ind = i
    return ind

def vector_to_cluster(k): #clear old clusters, finds the closest centroid for each vector and adds to cluster
    for i in range(k):
        clusters[i] = []
    for v in vector_list:
        ind = min_dist_centroid(v)
        clusters[ind].append(v)

def cluster_to_centroid(cluster): #calc centroids
    length = len(cluster) 
    res = [sum(elts) for elts in zip(*cluster)]
    for i in range(len(res)):
        res[i] = float(res[i]/length) 
    return res

def update_centroids(): #updates centroid for each cluster and checks if changed
    changed = 0
    for i in range(len(clusters)):
        new_centroid = cluster_to_centroid(clusters[i])
        if new_centroid != centroids[i]: 
            changed += 1
        centroids[i] = new_centroid
    return changed!=0
if(__name__ == "__main__"):
    main()
