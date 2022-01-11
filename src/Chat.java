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
    private JLabel user_already_friend;
    private JLabel user_does_not_exist;
    private JButton add_friend;
    private JButton delete_friend;
    private JButton send;
    private JButton load_chat;
    private JTextField add_friend_field;
    private JTextField send_msg_field;
    private JList friends_list;
    private JTextPane chat;
    private JScrollBar scrollPane;
    private String message = "";
    private Client client;
    private String userName;
    private String message_to="";


    public Chat(Client client){

        this.client = client;
        user_already_friend.setVisible(false);
        user_does_not_exist.setVisible(false);

        user_name.setText(client.getUsername());
        chat.setEditable(false);

        MyRunnable mr = new MyRunnable();
        Thread thread = new Thread(mr);
        thread.start();
        load_list_friends(friends_list,client.getFriends_list());

        send.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                user_already_friend.setVisible(false);
                user_does_not_exist.setVisible(false);
                try {
                    if(friends_list.getSelectedIndex()==-1){
                        JOptionPane.showMessageDialog(new JFrame(), "Proszę wybrać użytkownika z listy znajomych", "Error", JOptionPane.ERROR_MESSAGE);
                    }
                    else {

                        message_to = client.getFriend(friends_list.getSelectedIndex());
                        client.send_message(message_to.trim(), send_msg_field.getText());
                        send_msg_field.setText("");
                        chat.setText(client.load_conversation(message_to.trim()));
                    }
                } catch (IOException ex) {
                    //ex.printStackTrace();
                }
            }
        });

        add_friend.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                user_already_friend.setVisible(false);
                user_does_not_exist.setVisible(false);
                userName = add_friend_field.getText();
                String message="";
                try {
                    message = client.add_friend(userName);
                } catch (IOException ioException) {
                    ioException.printStackTrace();
                }
                if(message.equals("User does not exist")){
                    user_does_not_exist.setVisible(true);
                }
                else if(message.equals("User is already a friend")){
                    user_already_friend.setVisible(true);
                }
                load_list_friends(friends_list,client.getFriends_list());
                add_friend_field.setText("");

            }
        });

        delete_friend.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                user_already_friend.setVisible(false);
                user_does_not_exist.setVisible(false);
                userName = add_friend_field.getText();

                String message="";
                try {
                    message = client.delete_friend(userName);
                } catch (IOException ioException) {
                    ioException.printStackTrace();
                }
                if(message.equals("User does not exist")){
                    user_does_not_exist.setVisible(true);
                }
                load_list_friends(friends_list,client.getFriends_list());

                add_friend_field.setText("");

            }
        });

        load_chat.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                user_already_friend.setVisible(false);
                user_does_not_exist.setVisible(false);

                try {
                    if(friends_list.getSelectedIndex()==-1){
                        JOptionPane.showMessageDialog(new JFrame(), "Proszę wybrać użytkownika z listy znajomych", "Error", JOptionPane.ERROR_MESSAGE);
                    }
                    else {
                        message_to=client.getFriend(friends_list.getSelectedIndex());
                        chat.setText(client.load_conversation(message_to.trim()));
                    }
                } catch (IOException ex) {
                    //ex.printStackTrace();
                }

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

    public void delete_name_from_list(String name, List friends){
        if(friends.contains(name)){
            friends.remove(name);
            load_list_friends(friends_list,client.getFriends_list());
        }

    }

    private void createUIComponents() {
        // TODO: place custom component creation code here
    }

    public class MyRunnable implements Runnable{
        @Override
        public void run(){

            while (true) {

                try {

                    message = client.receive_msg();
                    if (!message.equals("")) {
                        String message2 = message.substring(0,1);

                        if(message2.equals("a")){
                            message = message.substring(2);
                            check_name_in_list(message.trim(),client.getFriends_list());
                        }
                        else if(message2.equals("d")){
                            message = message.substring(2);
                            delete_name_from_list(message.trim(),client.getFriends_list());
                        }
                        else if(message2.equals("s")){
                            message = message.substring(2);
                            chat.setText(client.load_conversation(message.trim()));
                        }

                    }

                } catch (IOException e) {
                    e.printStackTrace();
                }

            }

        }
    }

}
