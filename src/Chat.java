import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class Chat {

    private JPanel panel;
    private JLabel user_name;
    private JButton add_friend;
    private JButton send;
    private JButton new_message;
    private JTextField add_friend_field;
    private JTextField send_msg_field;
    private JTextArea chat;
    private JList friends_list;
    private String message = "";
    private Client client;
    private String userName;

    public Chat(Client client){

        this.client = client;
        user_name.setText(client.getUsername());
        MyRunnable mr = new MyRunnable();
        Thread thread = new Thread(mr);
        thread.start();
        load_list_friends(friends_list,client.getFriends_list());

        send.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

            }
        });

        add_friend.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

                userName = add_friend_field.getText();
                try {
                    client.add_friend(userName);
                } catch (IOException ioException) {
                    ioException.printStackTrace();
                }
                load_list_friends(friends_list,client.getFriends_list());
                add_friend_field.setText("");


            }
        });

        new_message.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {


            }
        });


    }

    public void load_list_friends(JList friends_list, List<String> users_friends){
        DefaultListModel<String> model = new DefaultListModel<>();
        for(int i=0;i< users_friends.size();i++){
            model.addElement((users_friends.get(i)));
        }
        friends_list.setModel(model);

    }

    public JPanel get_panel(){return panel;}

    public void check_name_in_list(String name, List friends){
        if(!friends.contains(name)){
            friends.add(name);
            load_list_friends(friends_list,client.getFriends_list());
        }

    }

    public class MyRunnable implements Runnable{
        @Override
        public void run(){

            //System.out.println("WEJSCIE");

            while (true) {

                try {
                    message = client.receive_msg();

                    if (!message.equals("")) {

                        //System.out.println(message);
                        String message2 = message.substring(0,1); // literka
                        //System.out.println(message2);
                        if(message2.equals("a")){

                            message = message.substring(2); // imie
                            //System.out.println(message);
                            check_name_in_list(message,client.getFriends_list());
                        }

                    }

                } catch (IOException e) {
                    e.printStackTrace();
                }

            }

        }
    }

}
