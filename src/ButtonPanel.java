import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.net.Socket;
import java.util.Scanner;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JScrollPane;

import java.net.*;
import java.io.*;
import java.util.Scanner;

public class ButtonPanel extends JPanel{

    private JScrollPane scroll;
    private JButton bOk;
    private JButton bCancel;
    private JButton bConnect;
    private JButton bAddFriend;
    private JPanel buttonPanel;


    static class OK extends JButton{

        OK() {
            super("Ok");
            addActionListener((ActionEvent e) -> {

            });
        }

    }

    static class CANCEL extends JButton{

        CANCEL() {
            super("Cancel");
            addActionListener((ActionEvent e) -> {

            });
        }
    }

    static class ADD_FRIEND extends JButton{

        ADD_FRIEND() {
            super("Add friend");
            addActionListener((ActionEvent e) -> {

            });
        }

    }
    static class CONNECT extends JButton{

        CONNECT() {
            super("Connect");
            addActionListener((ActionEvent e) -> {

                String hostname = "127.0.0.1";
                int port = 1234;

                Scanner scanner = new Scanner(System.in);
                System.out.println("Enter username: ");
                String username = scanner.nextLine();

                Socket socket = null;
                try {
                    socket = new Socket(hostname, port);
                } catch (IOException ioException) {
                    ioException.printStackTrace();
                }

                Client client = new Client(socket, username);
                client.receive_message();
                client.send_message();

            });
        }

    }


}
