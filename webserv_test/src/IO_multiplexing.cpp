#include <iostream>
#include <vector>
#include <cstring>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080

//code uses poll() for I/O multiplexing. 
//The poll function is used to monitor multiple file descriptors (sockets) for events, 
//making it an I/O multiplexing mechanism. Here's how the code incorporates I/O multiplexing:

//To set up I/O multiplexing in your server:

    //1. Choose an I/O Model: Decide between select(), poll(), epoll() (Linux-specific), or kqueue() (BSD-specific). 
    //Each has its advantages depending on your use case.

    //2. Implement Non-blocking Sockets: Configure your sockets to non-blocking mode.
    //This allows your server to handle other tasks while waiting for I/O operations to complete.

    //3. Integrate with Your Server Loop: Incorporate the chosen I/O multiplexing function into your server's main loop. 
    //This function will monitor multiple file descriptors to see if they are ready for I/O operations (like reading or writing).

    //4. Handle I/O Events: When the I/O multiplexing function indicates a file descriptor is ready, 
    //perform the necessary read or write operations.

    //5. Error Handling: Implement robust error handling, ensuring 
    //that your server can handle multiple clients and various I/O scenarios without crashing.



int main() 
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
      
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //Sets the address family to AF_INET, which means IPv4 addresses will be used.
    address.sin_family = AF_INET;
    
    //Assigns the IP address for the socket. 
    address.sin_addr.s_addr = INADDR_ANY;

    //Sets the port number for the socket. 
    address.sin_port = htons(PORT);
      
    //Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Set the socket to listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Set the socket to non-blocking mode
    fcntl(server_fd, F_SETFL, O_NONBLOCK);



    std::vector<struct pollfd> fds;

    //This setup allows the server to use poll to monitor for incoming connection requests on server_fd. 
    //When poll indicates that there is an event on server_fd, it means a client is attempting to connect.
    fds.push_back({server_fd, POLLIN, 0});

    //infinite loop to test
    while (true) 
    {
        //for monitoring multiple file descriptors to see if any of them have an event or change of state
        int ret = poll(fds.data(), fds.size(), -1); // Timeout set to -1 for indefinite waiting

        if (ret < 0) {
            perror("poll");
            exit(EXIT_FAILURE);
        }

        //auto: This is an automatic type deduction. 
        for (auto &fd : fds) 
        {
            //fd.revents: This is the field in the pollfd structure that poll() sets to indicate what event occurred.
            //POLLIN: This is a flag that indicates there is data to read (i.e., a read event). 
            //It could mean a new connection request on a listening socket or available data on a connected socket.
            if (fd.revents & POLLIN) 
            {
                //create new_socket 
                if (fd.fd == server_fd) 
                {
                    // Accept a new connection
                    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
                    if (new_socket < 0) 
                    {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }
                    //to set the newly created socket (represented by new_socket) to operate in non-blocking mode:
                    fcntl(new_socket, F_SETFL, O_NONBLOCK);
                    fds.push_back({new_socket, POLLIN, 0});
                } 
                //current file descriptor is not the server's main socket,
                // it means data is ready to be read from the client.
                else 
                {
                    // Handle read from a client socket
                    //initialized to store incoming data from the client.
                    char buffer[1024] = {0};
                    //read up to 1024 bytes from the client socket (fd.fd) into buffer
                    ssize_t count = read(fd.fd, buffer, 1024);
                    if (count == -1) 
                    {
                        if (errno != EAGAIN) 
                        {
                            perror("read failed");
                            close(fd.fd);
                        }
                    } 
                    else if (count == 0) 
                    {
                        // Client has closed the connection
                        close(fd.fd);
                    } 
                    else 
                    {
                        // Process the read data
                        std::cout << "Message from client: " << buffer << std::endl;
                    }
                }
            }
        }
    }

    return 0;
}



/* int main() 
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
      
    // Creating socket file descriptor
    
    // AF_INET : the address family (IPv4 Internet protocols)
    // SOCK_STREAM : the socket type for TCP (as opposed to SOCK_DGRAM for UDP)
    // 0 : the default protocol (TCP for SOCK_STREAM).
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    // Forcefully attaching socket to the port 8080

    //server_fd : the file descriptor for the server socket.
    //SOL_SOCKET : the options are at the socket level.
    //SO_REUSEADDR and SO_REUSEPORT are used to allow the socket to bind to an address/port that is already in use
    //opt : variable used with the setsockopt function to enable the SO_REUSEADDR and SO_REUSEPORT options on the socket. 
    
    //When you pass &opt and sizeof(opt) to setsockopt, 
    //you're telling the function to use the value of opt (which is 1) 
    //to set the state of SO_REUSEADDR and SO_REUSEPORT. 
    //This enables the socket to bind to a port 
    //even if it is already used by a socket in TIME_WAIT state, enhancing the robustness of binding, 
    //especially in development and testing scenarios where servers are restarted frequently.
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //Sets the address family to AF_INET, which means IPv4 addresses will be used.
    address.sin_family = AF_INET;
    
    //Assigns the IP address for the socket. 
    
    //INADDR_ANY means the socket will be bound to all available interfaces. 
    //For a server, this typically means it can accept connections from any interface.
    address.sin_addr.s_addr = INADDR_ANY;

    //Sets the port number for the socket. 
    
    //htons(PORT) converts the port number from host byte order to network byte order. 
    //PORT is defined as 8080 in the code, so the server will listen on port 8080.
    address.sin_port = htons(PORT);
      
    // Bind the socket to the network address and port

    //The bind function is necessary for a server socket 
    //because it assigns a specific local address (IP address and port number) to the socket. 
    //Without binding, the socket doesn't have a defined address where clients can send data or connection requests. 
    //In a server-client model, the server must bind to a well-known address (like a specific port) 
    //so that clients know where to connect. 
    //The bind function essentially tells the operating system 
    //that the server intends to listen for incoming connections on the specified address. 
    //This is a critical step in setting up a server.

    //sizeof(address)) < 0 : if bind returns a value less than 0 (such as the port being already in use).
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Set the socket to listen for incoming connections
    
    //The listen function is called on the server's file descriptor (server_fd). 
    //3, specifies the maximum length for the queue of pending connections—essentially 
    //how many client connections can be waiting while the server is handling another connection.
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Set the socket to non-blocking mode
    
    //fcntl is a versatile function used to manipulate file descriptors.
    //server_fd is the file descriptor for the server socket.
    //F_SETFL is the command used to set the file status flags.
    //O_NONBLOCK is the flag set on the socket, which enables non-blocking mode.
    fcntl(server_fd, F_SETFL, O_NONBLOCK);



    std::vector<struct pollfd> fds;

    //This setup allows the server to use poll to monitor for incoming connection requests on server_fd. 
    //When poll indicates that there is an event on server_fd, it means a client is attempting to connect.
    
    //POLLIN: A flag indicating that we are interested in read events (like incoming connections).
    fds.push_back({server_fd, POLLIN, 0});

    //infinite loop to test
    while (true) 
    {
        //for monitoring multiple file descriptors to see if any of them have an event or change of state
        
        //fds.data(): This provides a pointer to the underlying array of pollfd structures in the fds vector. 
        //It represents the file descriptors that poll will monitor.
        //fds.size(): This is the number of file descriptors that poll will monitor.
        int ret = poll(fds.data(), fds.size(), -1); // Timeout set to -1 for indefinite waiting

        if (ret < 0) {
            perror("poll");
            exit(EXIT_FAILURE);
        }

        //auto: This is an automatic type deduction. 
        //It tells the compiler to infer the type of the variable fd from the type of elements contained in fds.
        //fd is a loop variable that represents each element in the fds vector. 
        for (auto &fd : fds) 
        {
            //fd.revents: This is the field in the pollfd structure that poll() sets to indicate what event occurred.
            //POLLIN: This is a flag that indicates there is data to read (i.e., a read event). 
            //It could mean a new connection request on a listening socket or available data on a connected socket.
            if (fd.revents & POLLIN) 
            {
                //fd: This is the variable representing the current pollfd struct instance in the loop.
                //.fd: This is a member of the pollfd struct, specifically the file descriptor field. 
                //     It stores the integer value of a file descriptor (like a socket).
                //fd.fd gives you access to the file descriptor part of each pollfd struct during each iteration of the loop. 
                
                //server_fd is the file descriptor of the server's main socket, which listens for incoming connection requests.
                
                //create new_socket 
                if (fd.fd == server_fd) 
                {
                    // Accept a new connection
                    
                    // accept() is called on the server's listening socket (server_fd) to accept an incoming connection request.
                    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
                    if (new_socket < 0) 
                    {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }
                    //to set the newly created socket (represented by new_socket) to operate in non-blocking mode:
                    fcntl(new_socket, F_SETFL, O_NONBLOCK);
                    fds.push_back({new_socket, POLLIN, 0});
                } 
                //current file descriptor is not the server's main socket,
                // it means data is ready to be read from the client.
                else 
                {
                    // Handle read from a client socket
                    //initialized to store incoming data from the client.
                    char buffer[1024] = {0};
                    //read up to 1024 bytes from the client socket (fd.fd) into buffer
                    ssize_t count = read(fd.fd, buffer, 1024);
                    if (count == -1) 
                    {
                        if (errno != EAGAIN) 
                        {
                            perror("read failed");
                            close(fd.fd);
                        }
                    } 
                    else if (count == 0) 
                    {
                        // Client has closed the connection
                        close(fd.fd);
                    } 
                    else 
                    {
                        // Process the read data
                        std::cout << "Message from client: " << buffer << std::endl;
                    }
                }
            }
        }
    }

    return 0;
} */
