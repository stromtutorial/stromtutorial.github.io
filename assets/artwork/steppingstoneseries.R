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

nheads <- 20000
nflips <- 40000
beta1 <- 1
beta2 <- 1
a <- .2
bigK <- 5
sn <- 10000
line_width <- 2
cex_axis <- 1.5
ncolors <- bigK+1
colors = sort(heat.colors(ncolors, alpha=1), decreasing=TRUE)

prior_term  <- lgamma(beta1) + lgamma(beta2) - lgamma(beta1+beta2)
like_term   <- lgamma(nflips+1) - lgamma(nheads+1) - lgamma(nflips-nheads+1)
other_term  <- lgamma(beta1+nheads) + lgamma(beta1+nflips-nheads) - lgamma(beta1+beta2+nflips)
logmarglike <- other_term + like_term - prior_term
sprintf("true log marg. like. = %.8f", logmarglike)
sprintf("true marg. like. = %.8f", exp(logmarglike))

bquantile <- function(k, K, a) {
    # Beta(a,1) quantiles, k = 1,...,K
    # B_k = (k/K)^{1/a}
    # a = .2, K=5
    # B_0 = (0/5)^{1/.2} = 0.0^5 = 0.00000 (cum. prob. = 0.0) prior
    # B_1 = (1/5)^{1/.2} = 0.2^5 = 0.00032 (cum. prob. = 0.2)
    # B_2 = (2/5)^{1/.2} = 0.4^5 = 0.01024 (cum. prob. = 0.4)
    # B_3 = (3/5)^{1/.2} = 0.6^5 = 0.07776 (cum. prob. = 0.6)
    # B_4 = (4/5)^{1/.2} = 0.8^5 = 0.32768 (cum. prob. = 0.8)
    # B_5 = (5/5)^{1/.2} = 1.0^5 = 1.00000 (cum. prob. = 1.0) posterior
    if (k==0) 0 else exp((log(k) - log(K))/a)
}

calcLnL <- function(p) {
    term1 <- lgamma(nflips+1) - lgamma(nheads+1) - lgamma(nflips-nheads+1)
    term2 <- nheads*log(p)
    term3 <- (nflips-nheads)*log(1-p)
    logL <- term1 + term2 + term3
    logL
}

logf <- function(p, b) {
    term1 <- b*calcLnL(p)
    term2 <- log(dbeta(p, beta1, beta2))
    term1+term2
}

f <- function(p, b) {
    exp(logf(p,b))
}

x <- seq(0.001, 0.999, 0.001)

pdf("../img/steppingstone-series.pdf")
plot(x, f(x, 0), type='l', bty="n", xaxt="n", yaxt="n", xlab='', ylab='', xlim=c(0,1), ylim=c(0,max(f(x,0))))
polygon(c(0,x,1), c(0,f(x,0),0), density=-1, col=colors[1])
print(0)
for (k in 1:bigK) {
    b <- exp((log(k) - log(bigK))/a)
    print(b)
    lines(x, f(x,b))
    polygon(c(0,x,1), c(0,f(x,b),0), density=-1, col=colors[k+1])
}
dev.off()

logss <- 0.0
for (k in 1:bigK) {
    pdf(sprintf("../img/steppingstone-series%d%d.pdf", k-1, k))
    bkminus1 <- bquantile(k-1,bigK,a)
    bk <- bquantile(k,bigK,a)

    # sample from bkminus1
    sx <- double(sn)
    sy <- double(sn)
    tally1 <- 0
    tally2 <- double(sn)
    logLmax <- 1
    for (i in 1:sn)
        {
        u <- runif(1,0,1)
        q <- qbeta(u, beta1 + nheads*bkminus1, beta2 + (nflips - nheads)*bkminus1)
        sx[i] <- q
        sy[i] <- runif(1, 0, f(q, b=bkminus1))
        if (sy[i] < f(q, b=bk)) {
            tally1 <- tally1 + 1
        }
        logL <- calcLnL(q)
        tally2[i] <- logL
        if (logLmax > 0 || logL > logLmax) {
            logLmax = logL
            }
        }
    logfrac1 <- log(tally1) - log(sn)

    # a + b + c   c (a/c + b/c + c/c)
    # --------- = -------------------
    #     3                3
    bdiff <- bk-bkminus1
    logfrac2 <- bdiff*logLmax + log(sum(exp(bdiff*(tally2-logLmax)))) - log(sn)

    cat(sprintf("distribution: Beta(%.3f,%.3f)\n", beta1 + nheads*bkminus1, beta2 + (nflips - nheads)*bkminus1))
    cat(sprintf("  bkminus1 = %g\n", bkminus1))
    cat(sprintf("        bk = %g\n", bk))
    cat(sprintf("   logLmax = %g\n", logLmax))
    cat(sprintf("  logfrac1 = %g\n", logfrac1))
    cat(sprintf("  logfrac2 = %g\n", logfrac2))
    cat(sprintf("  f(.5,b_{k-1}) = %g\n", f(.5, bkminus1)))
    logss <- logss + logfrac2
    cat(sprintf("  logss    = %g\n", logss))
    plot(x, f(x, bkminus1), type='l', lwd=2, bty="L", cex.axis=1.5, xlab='', ylab='', xlim=c(0,1), ylim=c(0,1))
    points(sx, sy, pch=20, cex=.5, col="orange")
    lines(x, f(x, bkminus1), lwd=2, col="black")
    lines(x, f(x, bk), lwd=2, col="black")
    dev.off()
}
sprintf("estimated marginal likelihood = %.8f", exp(logss))
