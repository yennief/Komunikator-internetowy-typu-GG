import javax.swing.*;
import javax.swing.border.EmptyBorder;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.IOException;
import java.net.Socket;

public class StartWindow {
    private JPanel panel;
    private JButton Login;
    private JButton Register;
    private JPasswordField password_field;
    private JTextField name_field;
    private JLabel login_error;
    private JLabel login_error2;
    private JLabel login_success;
    private JLabel password_error;
    private JLabel empty_error;
    private JLabel empty_error2;
    private JLabel user_exist;
    private JLabel to_many;
    private JTextField host;
    private JTextField port;
    private JFrame frame;
    private String userName;
    private String userPassword;
    private int get_port;
    private String get_host;
    private Socket socket = null;
    private Client client = null;


    public StartWindow() throws IOException {

        user_exist.setVisible(false);
        to_many.setVisible(false);
        empty_error.setVisible(false);
        empty_error2.setVisible(false);
        login_success.setVisible(false);

        Login.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

                try {

                    login_error.setVisible(false);
                    login_error2.setVisible(false);
                    password_error.setVisible(false);
                    empty_error.setVisible(false);
                    empty_error2.setVisible(false);
                    user_exist.setVisible(false);
                    userName = name_field.getText();
                    userPassword = String.valueOf(password_field.getPassword());
                    get_port = Integer.valueOf(port.getText());
                    get_host = host.getText();

                    try {
                        socket = new Socket(get_host, get_port);
                    } catch (IOException ioException) {
                        //ioException.printStackTrace();
                    }

                    client = new Client(socket, userName, userPassword);

                    String message = client.checkLogin();
                    if (message.equals("User not found")) {
                        login_error.setVisible(true);
                    } else if (message.equals("Wrong password")) {
                        password_error.setVisible(true);
                    } else if(message.equals("User is already logged in!")){
                        login_error2.setVisible(true);
                    }

                    else {

                        login_success.setVisible(true);
                        frame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
                        frame.addWindowListener(new WindowAdapter() {

                            @Override
                            public void windowClosing(WindowEvent e) {
                                int if_exit = JOptionPane.showOptionDialog(
                                        null, "Do you want to exit?",
                                        "Exit Confirmation", JOptionPane.YES_NO_OPTION,
                                        JOptionPane.QUESTION_MESSAGE, null, null, null);
                                if (if_exit == 0) {
                                    try {
                                        client.log_out(client.getUsername());
                                    } catch (IOException ex) {
                                        ex.printStackTrace();
                                    }
                                    System.exit(0);

                                }
                            }
                        });
                        Chat chat = new Chat(client);
                        frame.setContentPane(chat.get_panel());
                        frame.pack();
                        frame.setSize(500,400);
                        frame.setVisible(true);

                    }

                } catch (Exception ex) {
                    //ex.printStackTrace();
                    //System.out.println("Problem z logowaniem");
                    empty_error.setVisible(true);

                }
            }

        });

        Register.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

                user_exist.setVisible(false);
                to_many.setVisible(false);
                empty_error.setVisible(false);
                empty_error2.setVisible(false);
                login_error.setVisible(false);
                login_error2.setVisible(false);
                password_error.setVisible(false);
                login_success.setVisible(false);

                userName = name_field.getText();
                userPassword = String.valueOf(password_field.getPassword());
                get_port = Integer.valueOf(port.getText());
                get_host = host.getText();

                try {

                    try {
                        socket = new Socket(get_host, get_port);
                    } catch (IOException ioException) {
                        //ioException.printStackTrace();
                    }
                    String message="";
                    client = new Client(socket, userName, userPassword);
                    try {
                        message = client.register();
                    } catch (IOException ioException) {
                        ioException.printStackTrace();
                    }
                    if (message.equals("User already exists")) {
                        user_exist.setVisible(true);
                    }
                    else if(message.equals("Too many users")) {
                        to_many.setVisible(true);
                    }

                    else {

                        client.new_socket(get_host, get_port);
                        client.checkLogin();
                        Chat chat = new Chat(client);
                        frame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
                        frame.addWindowListener(new WindowAdapter() {

                            @Override
                            public void windowClosing(WindowEvent e) {
                                int if_exit = JOptionPane.showOptionDialog(
                                        null, "Do you want to exit?",
                                        "Exit Confirmation", JOptionPane.YES_NO_OPTION,
                                        JOptionPane.QUESTION_MESSAGE, null, null, null);
                                if (if_exit == 0) {
                                    try {
                                        client.log_out(client.getUsername());
                                    } catch (IOException ex) {
                                        ex.printStackTrace();
                                    }
                                    System.exit(0);

                                }
                            }
                        });

                        frame.setContentPane(chat.get_panel());
                        frame.pack();
                        frame.setSize(500,400);
                        frame.setVisible(true);

                    }

                } catch (Exception ex) {
                    //ex.printStackTrace();
                    //System.out.println("Problem z rejestracja");
                    empty_error2.setVisible(true);
                }
            }

        });

    }

    public void setFrame(JFrame frame) {
        this.frame = frame;
    }

    public JPanel getPanel() {
        return panel;
    }

    private void createUIComponents() {
        // TODO: place custom component creation code here
    }
}
