# Reads surface points from file, and creates a 3D plot.
#
library("rgl")

# Read surface
data<-scan(file="/localdisk/pKa/dotted.srf")

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
plot3d(x, y, z, type = "p")

