

import java.net.*;
import java.io.*;
import java.util.Scanner;

public class Main {

    public static void main(String [] args) throws IOException {
        if (args.length < 1) return;

        String hostname = args[0];
        int port = Integer.valueOf(args[1]);

        Scanner scanner = new Scanner(System.in);
        System.out.println("Enter username: ");
        String username = scanner.nextLine();

        Socket socket = new Socket(hostname, port);

        Client client = new Client(socket, username);
        client.receive_message();
        client.send_message();

    }
}
