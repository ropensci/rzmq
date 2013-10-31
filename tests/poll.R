library(rzmq)

# ZMQ inproc endpoint to use in tests cases.
test.ENDPOINT <- "inproc://poll"

# Testing helpers.
assert <- function(condition, message="Assertion Failed") if(!condition) stop(message)
assert.fails <- function(expr, message="Assertion Failed") {
    result <- try(expr, TRUE)
    assert(inherits(result, 'try-error'), message)
}

# Generate the power set of an input vector.
powerset <- function(x) do.call(c, Map(function(n) combn(x, n, simplify=FALSE), 1:length(x)))

# A basic test of poll functionality.
test.rzmq.poll.basic <- function() {
    ctx <- init.context()
    s.rep <- init.socket(ctx, "ZMQ_REP")
    s.req <- init.socket(ctx, "ZMQ_REQ")

    bind.socket(s.rep, test.ENDPOINT)
    connect.socket(s.req, test.ENDPOINT)

    pollrc <- poll.socket(list(s.rep), list("read"), timeout=0L)
    assert(pollrc[[1]]$read == FALSE, "Poll should return $read==FALSE")

    send.socket(s.req, "Hello")
    pollrc <- poll.socket(list(s.rep), list("read"), timeout=0L)
    assert(pollrc[[1]]$read == TRUE, "Poll should return $read==TRUE")

    receive.socket(s.rep)
    pollrc <- poll.socket(list(s.rep), list("read"), timeout=0L)
    assert(pollrc[[1]]$read == FALSE, "Poll should return $read==FALSE")
}

# Ensures that poll fails when supplied with invalid
# arguments.
test.rzmq.poll.invalidargs <- function() {
    ctx <- init.context()
    s.rep <- init.socket(ctx, "ZMQ_REP")
    bind.socket(s.rep, test.ENDPOINT)

    assert.fails(poll.socket(list(s.rep), list("read", "read")), "poll shall only accept socket and event lists of equal length.")
    assert.fails(poll.socket(list(s.rep), list("read2")), "poll shall only accept event names: read, write, and error.")
    assert.fails(poll.socket(list(s.rep), list(2)), "poll shall only accept event names: read, write, and error.")
    assert.fails(poll.socket(list(), list("read")), "poll shall not accept an empty list of sockets.")
}

# Tests the powerset of {read, write, error} to ensure
# poll returns the correct flags.
test.rzmq.poll.returntypes <- function() {
    ctx <- init.context()
    s.rep <- init.socket(ctx, "ZMQ_REP")
    bind.socket(s.rep, test.ENDPOINT)

    combinations <- powerset(c("read", "write", "error"))

    testEventInput <- function(events) {
        pollrc <- poll.socket(list(s.rep), list(events), timeout=0L)
        assert(all(names(pollrc[[1]]) == events), "poll shall only return logical flags for events that it has requested.")
    }
    for (events in combinations) testEventInput(events)
}

# Run tests.
test.rzmq.poll.basic()
test.rzmq.poll.invalidargs()
test.rzmq.poll.returntypes()
