setwd('/Users/plewis/Documents/websites/stromtutorial.github.io/assets/artwork')

scale <- function(x, desired_min = 0, desired_max = 1, desired_frac = 1)
    {
    minx <- min(x)
    maxx <- max(x)
    diffx <- maxx - minx
    desired_diff <- desired_max - desired_min

    # scale the x values
    scaled_x <- (x - minx)/diffx
    scaled_x <- desired_min + scaled_x*desired_diff*desired_frac

    # create tick labels for scaled axis
    scaled_labels <- pretty(c(minx, minx + diffx/desired_frac), 4)

    # create positions for ticks
    scaled_label_pos <- pretty(c(minx, minx + diffx/desired_frac), 4)
    scaled_diff <- (desired_max - desired_min)*desired_frac*max(scaled_label_pos)/maxx
    lowest <- scaled_label_pos[1]
    highest <- scaled_label_pos[length(scaled_label_pos)]
    scaled_label_pos <- (scaled_label_pos - lowest)/(highest - lowest)
    scaled_label_pos <- desired_min + scaled_label_pos*scaled_diff

    list(scaled_x, scaled_labels, scaled_label_pos)
    }

y <- 1
n <- 3
beta1 <- 1
beta2 <- 1
a <- .2
bigK <- 1
sn <- 10000
line_width <- 2
cex_axis <- 1.5
ncolors <- bigK+1
colors = sort(heat.colors(ncolors, alpha=1), decreasing=TRUE)

prior_term  <- lgamma(beta1) + lgamma(beta2) - lgamma(beta1+beta2)
like_term   <- lgamma(n+1) - lgamma(y+1) - lgamma(n-y+1)
other_term  <- lgamma(beta1+y) + lgamma(beta1+n-y) - lgamma(beta1+beta2+n)
logmarglike <- other_term + like_term - prior_term
sprintf("true marg. like. = %.5f", exp(logmarglike))

calcLnL <- function(p, y, n) {
    term1 <- lgamma(n+1) - lgamma(y+1) - lgamma(n-y+1)
    term2 <- y*log(p)
    term3 <- (n-y)*log(1-p)
    logL <- term1 + term2 + term3
    logL
}

f <- function(p, b) {
    term1 <- b*calcLnL(p, y, n)
    term2 <- log(dbeta(p, beta1, beta2))
    exp(term1+term2)
}

sx <- double(sn)
sy <- double(sn)

# sample from prior
tally1 <- 0
tally2 <- 0
for (i in 1:sn)
    {
    x <- runif(1,0,1)
    sx[i] <- x
    sy[i] <- runif(1, 0, f(x, b=0))
    if (sy[i] < f(x, b=1)) {
        tally1 <- tally1 + 1
    }
    numer <- f(x, b=1)
    denom <- f(x, b=0)
    tally2 <- tally2 + numer/denom
    }
sprintf("observed inside = %g", tally1)
sprintf("total darts     = %g", sn)
tally1 <- tally1/sn
tally2 <- tally2/sn
sprintf("observed fraction  = %g", tally1)
sprintf("observed fraction  = %g", tally1)
sprintf("estimated fraction = %g", tally2)

x <- seq(0.001, 0.999, 0.001)

#pdf("../img/steppingstone-basic-kernelonly.pdf")
#plot(x, f(x, 1), type='l', lwd=3, bty="n", xaxt="n", yaxt="n", cex.axis=1.5, xlab='', ylab='', xlim=c(0,1), ylim=c(0,max(f(x,0))))
#dev.off()

#pdf("../img/steppingstone-basic-nodarts.pdf")
#plot(x, f(x, 0), type='l', lwd=3, bty="n", xaxt="n", yaxt="n", cex.axis=1.5, xlab='', ylab='', xlim=c(0,1), ylim=c(0,max(f(x,0))))
#lines(x, f(x,0), lwd=3)
#lines(x, f(x,1), lwd=3)
#dev.off()

pdf("../img/steppingstone-basic-darts.pdf")
plot(x, f(x, 0), type='l', lwd=3, bty="n", xaxt="n", yaxt="n", cex.axis=1.5, xlab='', ylab='', xlim=c(0,1), ylim=c(0,max(f(x,0))))
points(sx, sy, pch=20, cex=.5, col="orange")
lines(x, f(x,0), lwd=3)
lines(x, f(x,1), lwd=3)
dev.off()

