#include "buffer.h"
#include "helpers.h"

int main() {
    // IP-ul serverului
    char ip[] = "3.8.116.10";
    // Bufferul in care vor fi citite comenzile
    char command[CMDLEN];
    // Cookie-ul utilizatorului curent
    std::string session_cookie;
    // Tokenul JWT pentru accesul in biblioteca
    std::string library_jwt_token;
    // Variabila folosita pentru a afla daca utilizatorul are acces la biblioteca
    bool is_in_library = false;

    while (true) {
        // Citim comanda
        std::cin >> command;

        if (strcmp(command, "register") == 0) {
            // JSON in care vom retine datele utilizatorului
            json register_data;
            char register_url[] = "/api/v1/tema/auth/register";
            char content_type[] = "application/json";
            char username[LINELEN];
            char password[LINELEN];
            char line[LINELEN];
            char *message = (char *) calloc(BUFLEN, sizeof(char));
            int sockfd = open_connection(ip, PORT, AF_INET, SOCK_STREAM, 0);

            // Verificam daca a fost realizata cu succes conexiunea
            if (sockfd < 0) {
                perror("Connection error.\n");
                exit(EXIT_FAILURE);
            }

            // Completam mesajul HTTP
            sprintf(line, "POST %s %s", register_url, HTTP_TYPE);
            compute_message(message, line);

            sprintf(line, "Host: %s", HOST);
            compute_message(message, line);

            sprintf(line, "Content-Type: %s", content_type);
            compute_message(message, line);

            // Citim numele utilizatorului
            std::cout << "Enter register details" << std::endl;
            std::cout << "Username: ";
            std::cin.ignore();
            std::cin.getline(username, LINELEN);

            // Citim parola utilizatorului
            std::cout << "Password: ";
            std::cin >> password;

            register_data["username"] = username;
            register_data["password"] = password;

            // Calculam lungimea continutului
            sprintf(line, "Content-Length: %lu", register_data.dump().length());
            compute_message(message, line);

            compute_message(message, "");

            sprintf(line, "%s", register_data.dump().c_str());
            compute_message(message, line);

            // Trimitem mesajul catre server
            send_to_server(sockfd, message);

            // Raspunsul serverului
            char *response = receive_from_server(sockfd);
            char *error_string = strstr(response, ERROR_STRING);

            // Verificam daca avem o eroare
            if (error_string) {
                std::cout << error_string << std::endl;
            } else {
                std::cout << "User created successfully" << std::endl;
            }

            // Eliberam memoria
            free(message);
            close(sockfd);
        } else if (strcmp(command, "login") == 0) {
            // JSON in care vom retine datele utilizatorului
            json login_data;
            char login_url[] = "/api/v1/tema/auth/login";
            char content_type[] = "application/json";
            char username[LINELEN];
            char password[LINELEN];
            char line[LINELEN];
            char *message = (char *) calloc(BUFLEN, sizeof(char));
            int sockfd = open_connection(ip, PORT, AF_INET, SOCK_STREAM, 0);

            // Verificam daca a fost realizata cu succes conexiunea
            if (sockfd < 0) {
                perror("Connection error.\n");
                exit(EXIT_FAILURE);
            }

            // Completam mesajul HTTP
            sprintf(line, "POST %s %s", login_url, HTTP_TYPE);
            compute_message(message, line);

            sprintf(line, "Host: %s", HOST);
            compute_message(message, line);

            sprintf(line, "Content-Type: %s", content_type);
            compute_message(message, line);

            // Citim numele utilizatorului
            std::cout << "Enter login details" << std::endl;
            std::cout << "Username: ";
            std::cin.ignore();
            std::cin.getline(username, LINELEN);

            // Citim parola utilizatorului
            std::cout << "Password: ";
            std::cin >> password;

            login_data["username"] = username;
            login_data["password"] = password;

            sprintf(line, "Content-Length: %lu", login_data.dump().length());
            compute_message(message, line);

            compute_message(message, "");

            // Calculam lungimea continutului
            sprintf(line, "%s", login_data.dump().c_str());
            compute_message(message, line);

            // Trimitem mesajul catre server
            send_to_server(sockfd, message);

            // Raspunsul serverului
            char *response = receive_from_server(sockfd);
            char *error_string = strstr(response, ERROR_STRING);

            // Verificam daca avem o eroare
            if (error_string) {
                std::cout << error_string << std::endl;
            } else {
                std::string cookie_str = "connect.sid=";
                std::string response_str(response);
                int start_index = response_str.find(cookie_str);
                int stop_index = response_str.find("Pa", start_index);

                session_cookie = response_str.substr(start_index,
                        stop_index - start_index - 2);

                std::cout << "Logged in successfully" << std::endl;
            }

            // Eliberarea memoriei
            free(message);
            close(sockfd);
        } else if (strcmp(command, "enter_library") == 0) {
            char access_url[] = "/api/v1/tema/library/access";
            char line[LINELEN];
            char *message = (char *) calloc(BUFLEN, sizeof(char));
            int sockfd = open_connection(ip, PORT, AF_INET, SOCK_STREAM, 0);

            // Verificam daca a fost realizata cu succes conexiunea
            if (sockfd < 0) {
                perror("Connection error.\n");
                exit(EXIT_FAILURE);
            }

            // Completam mesajul HTTP
            sprintf(line, "GET %s %s", access_url, HTTP_TYPE);
            compute_message(message, line);

            sprintf(line, "Host: %s", HOST);
            compute_message(message, line);

            sprintf(line, "Cookie: %s", session_cookie.c_str());
            compute_message(message, line);

            compute_message(message, "");

            // Trimitem mesajul catre server
            send_to_server(sockfd, message);

            // Raspunsul serverului
            char *response = receive_from_server(sockfd);
            char *error_string = strstr(response, ERROR_STRING);

            // Verificam daca avem o eroare
            if (error_string) {
                std::cout << error_string << std::endl;
            } else {
                char *jwt_token = strstr(response, "{\"token");
                std::string jwt_token_str(jwt_token);

                int start_index = jwt_token_str.find(":\"");
                int stop_index = jwt_token_str.find("\"}");

                library_jwt_token = jwt_token_str.substr(start_index + 2,
                        stop_index - start_index - 2);

                is_in_library = true;

                std::cout << "You are in the library" << std::endl;
            }

            // Eliberam memoria
            free(message);
            close(sockfd);
        } else if (strcmp(command, "get_books") == 0) {
            // Verficam daca utilizatorul se afla in biblioteca
            if (!is_in_library) {
                std::cout << "You are not in the library" << std::endl;
                continue;
            }

            char books_url[] = "/api/v1/tema/library/books";
            char line[LINELEN];
            char *message = (char *) calloc(BUFLEN, sizeof(char));
            int sockfd = open_connection(ip, PORT, AF_INET, SOCK_STREAM, 0);

            // Verificam daca a fost realizata cu succes conexiunea
            if (sockfd < 0) {
                perror("Connection error.\n");
                exit(EXIT_FAILURE);
            }

            // Completam mesajul HTTP
            sprintf(line, "GET %s %s", books_url, HTTP_TYPE);
            compute_message(message, line);

            sprintf(line, "Host: %s", HOST);
            compute_message(message, line);

            sprintf(line, "Authorization: Bearer %s", library_jwt_token.c_str());
            compute_message(message, line);

            compute_message(message, "");

            // Trimitem mesajul catre server
            send_to_server(sockfd, message);

            // Raspunsul serverului
            char *response = receive_from_server(sockfd);
            char *error_string = strstr(response, ERROR_STRING);

            // Verificam daca avem o eroare
            if (error_string) {
                std::cout << error_string << std::endl;
            } else {
                std::cout << strstr(response, "[") << std::endl;
            }

            // Eliberam memoria
            free(message);
            close(sockfd);
        } else if (strcmp(command, "get_book") == 0) {
            // Verficam daca utilizatorul se afla in biblioteca
            if (!is_in_library) {
                std::cout << "You are not in the library" << std::endl;
                continue;
            }

            char books_url[] = "/api/v1/tema/library/books";
            char line[LINELEN];
            char id[LINELEN];
            char *message = (char *) calloc(BUFLEN, sizeof(char));
            int sockfd = open_connection(ip, PORT, AF_INET, SOCK_STREAM, 0);

            // Verificam daca a fost realizata cu succes conexiunea
            if (sockfd < 0) {
                perror("Connection error.\n");
                exit(EXIT_FAILURE);
            }

            // Citim ID-ul cartii
            std::cout << "Enter book ID: ";
            std::cin >> id;

            // Completam mesajul HTTP
            sprintf(line, "GET %s/%s %s", books_url, id, HTTP_TYPE);
            compute_message(message, line);

            sprintf(line, "Host: %s", HOST);
            compute_message(message, line);

            sprintf(line, "Authorization: Bearer %s", library_jwt_token.c_str());
            compute_message(message, line);

            compute_message(message, "");

            // Trimitem mesajul catre server
            send_to_server(sockfd, message);

            // Raspunsul serverului
            char *response = receive_from_server(sockfd);
            char *error_string = strstr(response, ERROR_STRING);

            // Verificam daca avem o eroare
            if (error_string) {
                std::cout << error_string << std::endl;
            } else {
                std::cout << strstr(response, "[{") << std::endl;
            }

            // Eliberam memoria
            free(message);
            close(sockfd);
        } else if (strcmp(command, "add_book") == 0) {
            // Verficam daca utilizatorul se afla in biblioteca
            if (!is_in_library) {
                std::cout << "You are not in the library" << std::endl;
                continue;
            }

            // JSON in care vom retine datele cartii
            json book_details;
            char books_url[] = "/api/v1/tema/library/books";
            char content_type[] = "application/json";
            char line[LINELEN];
            char title[LINELEN];
            char author[LINELEN];
            char genre[LINELEN];
            char publisher[LINELEN];
            char page_num[LINELEN];
            char *message = (char *) calloc(BUFLEN, sizeof(char));
            int sockfd = open_connection(ip, PORT, AF_INET, SOCK_STREAM, 0);

            // Verificam daca a fost realizata cu succes conexiunea
            if (sockfd < 0) {
                perror("Connection error.\n");
                exit(EXIT_FAILURE);
            }

            // Completam mesajul HTTP
            sprintf(line, "POST %s %s", books_url, HTTP_TYPE);
            compute_message(message, line);

            sprintf(line, "Host: %s", HOST);
            compute_message(message, line);

            sprintf(line, "Authorization: Bearer %s", library_jwt_token.c_str());
            compute_message(message, line);

            sprintf(line, "Content-Type: %s", content_type);
            compute_message(message, line);

            // Citim titlul cartii
            std::cout << "Add book" << std::endl;
            std::cout << "Title: ";
            std::cin.ignore();
            std::cin.getline(title, LINELEN);

            // Citim autorul cartii
            std::cout << "Author: ";
            std::cin.getline(author, LINELEN);

            // Citim genul cartii
            std::cout << "Genre: ";
            std::cin.getline(genre, LINELEN);

            // Citim editura cartii
            std::cout << "Publisher: ";
            std::cin.getline(publisher, LINELEN);

            // Citim numarul de pagini
            std::cout << "Page Count: ";
            std::cin >> page_num;

            int page_no = 0;

            if ((page_no = strtol(page_num, NULL, BASE)) <= 0) {
                std::cout << "Not a valid page number" << std::endl;
                free(message);
                continue;
            }

            // Adaugam datele citite in JSON
            book_details["title"] = title;
            book_details["author"] = author;
            book_details["genre"] = genre;
            book_details["page_count"] = page_no;
            book_details["publisher"] = publisher;

            // Calculam lungimea continutului
            sprintf(line, "Content-Length: %lu", book_details.dump().length());
            compute_message(message, line);

            compute_message(message, "");

            sprintf(line, "%s", book_details.dump().c_str());
            compute_message(message, line);

            // Trimitem mesajul catre server
            send_to_server(sockfd, message);

            // Raspunsul serverului
            char *response = receive_from_server(sockfd);
            char *error_string = strstr(response, ERROR_STRING);

            // Verificam daca sunt prea multe requesturi
            char *too_many_requests_error = strstr(response, TOO_MANY_REQUESTS_STRING);

            // Verificam daca avem o eroare
            if (error_string) {
                std::cout << error_string << std::endl;
            } else if (too_many_requests_error) {
                std::cout << TOO_MANY_REQUESTS_STRING << std::endl;
            } else {
                std::cout << "Book added successfully" << std::endl;
            }

            // Eliberam memoria
            free(message);
            close(sockfd);
        } else if (strcmp(command, "delete_book") == 0) {
            // Verficam daca utilizatorul se afla in biblioteca
            if (!is_in_library) {
                std::cout << "You are not in the library" << std::endl;
                continue;
            }

            char books_url[] = "/api/v1/tema/library/books";
            char line[LINELEN];
            char id[LINELEN];
            char *message = (char *) calloc(BUFLEN, sizeof(char));
            int sockfd = open_connection(ip, PORT, AF_INET, SOCK_STREAM, 0);

            // Verificam daca a fost realizata cu succes conexiunea
            if (sockfd < 0) {
                perror("Connection error.\n");
                exit(EXIT_FAILURE);
            }

            // Citim ID-ul cartii
            std::cout << "Enter book ID: ";
            std::cin >> id;

            // Completam mesajul HTTP
            sprintf(line, "DELETE %s/%s %s", books_url, id, HTTP_TYPE);
            compute_message(message, line);

            sprintf(line, "Host: %s", HOST);
            compute_message(message, line);

            sprintf(line, "Authorization: Bearer %s", library_jwt_token.c_str());
            compute_message(message, line);

            compute_message(message, "");

            // Trimitem mesajul catre server
            send_to_server(sockfd, message);

            // Raspunsul serverului
            char *response = receive_from_server(sockfd);
            char *error_string = strstr(response, ERROR_STRING);

            // Verificam daca avem o eroare
            if (error_string) {
                std::cout << error_string << std::endl;
            } else {
                std::cout << "Book deleted successfully" << std::endl;
            }

            // Eliberam memoria
            free(message);
            close(sockfd);
        } else if (strcmp(command, "logout") == 0) {
            char access_url[] = "/api/v1/tema/auth/logout";
            char line[LINELEN];
            char *message = (char *) calloc(BUFLEN, sizeof(char));
            int sockfd = open_connection(ip, PORT, AF_INET, SOCK_STREAM, 0);

            // Verificam daca a fost realizata cu succes conexiunea
            if (sockfd < 0) {
                perror("Connection error.\n");
                exit(EXIT_FAILURE);
            }

            // Completam mesajul HTTP
            sprintf(line, "GET %s %s", access_url, HTTP_TYPE);
            compute_message(message, line);

            sprintf(line, "Host: %s", HOST);
            compute_message(message, line);

            sprintf(line, "Cookie: %s", session_cookie.c_str());
            compute_message(message, line);

            compute_message(message, "");

            // Trimitem mesajul catre server
            send_to_server(sockfd, message);

            // Raspunsul serverului
            char *response = receive_from_server(sockfd);
            char *error_string = strstr(response, ERROR_STRING);

            // Verificam daca avem o eroare
            if (error_string) {
                std::cout << error_string << std::endl;
            } else {
                library_jwt_token.clear();
                is_in_library = false;
                std::cout << "You are logged out" << std::endl;
            }

            // Eliberam memoria
            free(message);
            close(sockfd);
        } else if (strcmp(command, "exit") == 0) {
            // Intrerupem bucla infinita
            std::cout << "Exiting program..." << std::endl;
            break;
        } else {
            std::cout << "Invalid command" << std::endl;
        }
    }

    return 0;
}
