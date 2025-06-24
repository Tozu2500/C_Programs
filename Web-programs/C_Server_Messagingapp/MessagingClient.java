import java.io.*;
import java.net.*;
import java.util.Scanner;

public class MessagingClient {

    private static final String SERVER_HOST = "localhost";

    private static final int SERVER_PORT = 8080;

    private Socket socket;
    private BufferedReader reader;
    private PrintWriter writer;
    private Scanner scanner;
    private String username;
    private volatile boolean running = false;

    public MessagingClient() {
        scanner = new Scanner(System.in);
    }

    public void start() {
        try {
            // Getting the username
            System.out.println("Enter your username:");
            username = scanner.nextLine().trim();

            if (username.length() < 2 || username.length() > 30) {
                System.out.println("Username must be between 2 and 30 characters long.");
                return;
            }

            // Connecting to the server
            socket = new Socket(SERVER_HOST, SERVER_PORT);
            reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            writer = new PrintWriter(socket.getOutputStream(), true);

            writer.println(username); // Send username to server

            System.out.println("Connected to the server as " + username);
            System.out.println("Type your messages below. Commands:");
            System.out.println("  /quit - Leave the chat");
            System.out.println("  /users - List online users");
            System.out.println("=======================================");
            
            running = true;

            // Start message receiver thread
            Thread receiverThread = new Thread(this::receiveMessages);
            receiverThread.setDaemon(true);
            receiverThread.start();

            sendMessages();

        } catch (IOException e) {
             System.err.println("Error connecting to the server: " + e.getMessage());
        } finally {
            cleanup();
        }
    }

    private void receiveMessages() {
        try {
            String message;
            while (running && (message = reader.readLine()) != null) {
                // Clear current input lines and print the message received.
                System.out.print("\r\033[K"); // Clear the current line
                System.out.println(message);
                System.out.print("> ");
                System.out.flush();
            }
        } catch (IOException e) {
            if (running) {
                System.err.println("Connection lost to the server. " + e.getMessage());
            }
        }
    }

    private void sendMessages() {
        System.out.print("> ");

        while (running) {
            if (scanner.hasNextLine()) {
                String message = scanner.nextLine().trim();

                if (message.isEmpty()) {
                    System.out.print("> ");
                    continue; // Skip empty messages 
                }

                if (message.equals("/quit")) {
                    running = false;
                    writer.println(message);
                    break; // Exit the loop
                }

                writer.println(message); // Send message to server
                System.out.print("> "); // Prompt for next message
            }
        }
    }

    private void cleanup() {
        running = false;

        try {
            if (writer != null) {
                writer.close();
            }

            if (reader != null) {
                reader.close();
            }

            if (socket != null && !socket.isClosed()) {
                socket.close();
            }

            if (scanner != null) {
                scanner.close();
            }
        } catch (IOException e) {
            System.err.println("Error during cleanup, some open resources could not be closed: " + e.getMessage());
        }
        System.out.println("Disconnected from the server. Goodbye!");
    }

    public static void main(String[] args) {
        
        // Start the messaging client
        System.out.println(" === Messaging Client === ");
        
        MessagingClient client = new MessagingClient();

        // Handle CTRL.C gracefully
        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            System.out.println("\nShutting down client...");
            client.cleanup();
        }));

        client.start(); // Start the client
    } 

}