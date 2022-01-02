import java.awt.*;
import java.net.*;
import java.io.*;
import java.util.Scanner;
import javax.swing.*;

public class Application extends JFrame {

    private JButton bOk;
    private JButton bCancel;
    private JButton bConnect;
    private JButton bAddFriend;

    public Application() throws IOException {
        //if (args.length < 1) return;
        super("Test akcji");
        JPanel buttonPanel_c = new JPanel(new BorderLayout());
        JPanel buttonPanel_s = new JPanel(new FlowLayout());
        JPanel buttonPanel_e = new JPanel(new BorderLayout());
        add(buttonPanel_c, BorderLayout.CENTER);
        add(buttonPanel_s,BorderLayout.SOUTH);
        add(buttonPanel_e,BorderLayout.EAST);

        bOk = new ButtonPanel.OK();

        bCancel = new ButtonPanel.CANCEL();

        bConnect = new ButtonPanel.CONNECT();
        buttonPanel_s.add(bConnect);

        bAddFriend = new ButtonPanel.ADD_FRIEND();
        buttonPanel_e.add(bAddFriend, BorderLayout.SOUTH);

        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(750,750);
        setVisible(true);




        /*String hostname = "127.0.0.1";
        int port = 1234;

        Scanner scanner = new Scanner(System.in);
        System.out.println("Enter username: ");
        String username = scanner.nextLine();

        Socket socket = new Socket(hostname, port);

        Client client = new Client(socket, username);
        client.receive_message();
        client.send_message();*/

    }
}
