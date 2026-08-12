#include <iostream>
#include <string>
#include "httplib.h"

double calculate(double x, double y)
{
    // An actual process for the HTTP requestè
    return x + y;
}

int main()
{

    // Object
    httplib::Server server;

    // Define an Endpoint for Get/Calculate
    server.Get("/calculate",
        [](const httplib::Request& req, httplib::Response& res)
        {
            try {
                // Retrieve parameters from the HTTP request
                double x = std::stod(req.get_param_value("x"));
                double y = std::stod(req.get_param_value("y"));

                // Execute the calculation
                double result = calculate(x, y);

                // Set the results to HTTP response
                res.set_content(
                    std::to_string(result),
                    "text/plain"
                );
            }

            // Exception. Set 400 if an exception occurs
            catch (const std::exception& e) {
                res.status = 400;
                res.set_content(
                    std::string("Bad Request: ") + e.what(),
                    "text/plain"
                );
            }
        }
    );

    // Output a message on the console when the server starts
    std::cout << "Server started on port 8080" << std::endl;

    // Listen for HTTP requests on 8080 port
    server.listen("0.0.0.0", 8080);

    return 0;
}
