import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class Chat {

    private JPanel panel;
    private JButton add_friend;
    private JButton send;
    private JButton new_message;
    private JTextField add_friend_field;
    private JTextField send_msg_field;
    private JTextArea chat;
    private JList friends_list;
    private String message = "";
    private Client client;

    public Chat(Client client){

        this.client = client;
        MyRunnable mr = new MyRunnable();
        Thread thread = new Thread(mr);
        thread.start();

        send.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

            }
        });

        add_friend.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

            }
        });

        new_message.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {


            }
        });



    }

    public JPanel get_panel(){return panel;}

    public class MyRunnable implements Runnable{
        @Override
        public void run(){


            while (true) {

                try {
                    message = client.receive_msg();

                    if (!message.equals("")) {

                    }

                } catch (IOException e) {
                    e.printStackTrace();
                }

            }

        }
    }

}
