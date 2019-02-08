Nickolas Gough, nvg081, 11181823


Running sender-a:

    make sender-a
    ./sender-a <receiver name> <receiver port> <window size> <timeout>

Example:

    make sender-a
    ./sender-a tux8.usask.ca 30001 10 5

Notes:

    The sender will print a prompt when it is ready to receive input, and
    will print output indicating the status of its processing when handling
    an acknowledgement.


Running sender-b:

    make sender-b
    ./sender-b <receiver name> <receiver port> <window size> <timeout>

Example:

    make sender-b
    ./sender-b tux8.usask.ca 30001 10 5

Notes:

    The sender will print a prompt when it is ready to receive input, and
    will print output indicating the status of its processing when handling
    an acknowledgement.


Running receiver-a:

    make receiver-a
    ./receiver-a <port number> <probability>

Example:

    make receiver-a
    ./receiver-a 30001 50

Notes:

    The receiver will print a prompt when it is ready to receive input, and
    will print output indicating the status of its processing when handling
    a received frame.


Running receiver-b:

    make receiver-b
    ./receiver-b <port number> <buffer size> <probability>

Example:

    make receiver-b
    ./receiver-b 30001 5 50

Notes:

    The receiver will print a prompt when it is ready to receive input, and
    will print output indicating the status of its processing when handling
    a received frame.
