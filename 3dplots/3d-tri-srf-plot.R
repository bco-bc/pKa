# Reads triangulated surface from file, and creates a 3D plot.
#
library("rgl")

# Read dotted surface
data<-scan(file="/home/ajuffer/PT-CGMD/work/dotted.srf")
#data<-scan(file="/localdisk/pKa/dotted.srf")

# Create x,y,z vectors.
nsp<-data[1]               #  Number of surface points
x <- rep(0.0, nsp)
y <- rep(0.0, nsp)
z <- rep(0.0, nsp)
for (j in 1:nsp) {
  j3 <- 3 * j
  for (k in 1:3) {
    x[j] <- data[j3-1]
    y[j] <- data[j3]
    z[j] <- data[j3 +1]
  }
}

# Draw dotted surface.
open3d()
plot3d(x, y, z, type = "p", col = "steelblue")

# Read triangulated surface
data<-scan(file="/home/ajuffer/PT-CGMD/work/triangulated.srf")
#data<-scan(file="/localdisk/pKa/triangulated.srf")

# Draw edges.
nvertices<-data[1]
ntr <- 7 * nvertices + 2
ntriangles <- data[ntr]
ne <- ntr + ntriangles * 3 + 1
nedges <- data[ne]
x <- rep(0.0, 2 * nedges)
y <- rep(0.0, 2 * nedges)
z <- rep(0.0, 2 * nedges)

start <- ne + 1   # Index of first edge, each line holds three.
#j <- 1
for (j in 1:nedges) {
  j2 = 2 * j - 2
  
  from <- data[start + j2]
  index <- 2 + 7 * (from - 1)
  x[j2 + 1] <- data[index + 1]
  y[j2 + 1] <- data[index + 2]
  z[j2 + 1] <- data[index + 3]
  
  to <- data[start + j2 + 1]
  index <- 2 + 7 * (to - 1)
  x[j2 + 2] <- data[index + 1]
  y[j2 + 2] <- data[index + 2]
  z[j2 + 2] <- data[index + 3]
}

# Draw line segments.
segments3d(x, y, z, col = "red")
