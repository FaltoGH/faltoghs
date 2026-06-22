import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Collections;
import java.util.List;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JTextField;

import java.awt.AWTException;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Image;
import java.awt.Insets;
import java.awt.SystemTray;
import java.awt.Toolkit;
import java.awt.TrayIcon;
import java.awt.TrayIcon.MessageType;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import com.google.api.client.auth.oauth2.Credential;
import com.google.api.client.extensions.java6.auth.oauth2.AuthorizationCodeInstalledApp;
import com.google.api.client.extensions.jetty.auth.oauth2.LocalServerReceiver;
import com.google.api.client.googleapis.auth.oauth2.GoogleAuthorizationCodeFlow;
import com.google.api.client.googleapis.auth.oauth2.GoogleClientSecrets;
import com.google.api.client.googleapis.javanet.GoogleNetHttpTransport;
import com.google.api.client.http.javanet.NetHttpTransport;
import com.google.api.client.json.JsonFactory;
import com.google.api.client.json.gson.GsonFactory;
import com.google.api.client.util.store.FileDataStoreFactory;
import com.google.api.services.sheets.v4.Sheets;
import com.google.api.services.sheets.v4.SheetsScopes;
import com.google.api.services.sheets.v4.model.ValueRange;

public class GoogleSpreadsheetObserver extends JFrame {
    public final JTextField row = new JTextField("1");
    public final JTextField col = new JTextField("A");
    public final JTextField text = new JTextField("Student Name");
    public final JTextField id = new JTextField("1BxiMVs0XRA5nFMdKvBdBZjgmUUqptlbs74OgvE2upms");
    public final JTextField sheet = new JTextField("Class Data");
    public final JButton button = new JButton("Observe");
    public final JLabel currentText = new JLabel();
    private static final JsonFactory JSON_FACTORY = GsonFactory.getDefaultInstance();
    /**
     * Global instance of the scopes required by this quickstart.
     * If modifying these scopes, delete your previously saved tokens/ folder.
     */
    private static final List<String> SCOPES = Collections.singletonList(SheetsScopes.SPREADSHEETS_READONLY);
    private static NetHttpTransport HTTP_TRANSPORT;
    private static Sheets service;
    private static final String APPLICATION_NAME = "Google Sheets API Java Observer";
    private static final String TOKENS_DIRECTORY_PATH = "tokens";
    private static final String CREDENTIALS_FILE_PATH = "/credentials.json";

    /**
     * Prints the names and majors of students in a sample spreadsheet:
     * https://docs.google.com/spreadsheets/d/1BxiMVs0XRA5nFMdKvBdBZjgmUUqptlbs74OgvE2upms/edit
     */
    public static String readCell(String spreadsheetId, String range) {
        try {
            // Build a new authorized API client service.
            if (HTTP_TRANSPORT == null) {
                HTTP_TRANSPORT = GoogleNetHttpTransport.newTrustedTransport();
            }
            // final String spreadsheetId = "1BxiMVs0XRA5nFMdKvBdBZjgmUUqptlbs74OgvE2upms";
            // final String range = "Class Data!A2:E";
            if (service == null) {
                service = new Sheets.Builder(HTTP_TRANSPORT, JSON_FACTORY, getCredentials(HTTP_TRANSPORT))
                        .setApplicationName(APPLICATION_NAME)
                        .build();
            }
            ValueRange response = service.spreadsheets().values().get(spreadsheetId, range).execute();
            List<List<Object>> values = response.getValues();
            if (values == null || values.isEmpty()) {
                System.err.println("values are null or empty!");
            } else {
                List<Object> row = values.get(0);
                Object value = row.get(0);
                return value.toString();
            }
        } catch (Exception ex) {
            System.err.println(ex);
        }
        return null;
    }

    public static void alert(String text) throws AWTException {
        // Obtain only one instance of the SystemTray object
        SystemTray tray = SystemTray.getSystemTray();

        // If the icon is a file
        Image image = Toolkit.getDefaultToolkit().createImage("icon.png");
        // Alternative (if the icon is on the classpath):
        // Image image =
        // Toolkit.getDefaultToolkit().createImage(getClass().getResource("icon.png"));

        TrayIcon trayIcon = new TrayIcon(image, "");
        // Let the system resize the image if needed
        trayIcon.setImageAutoSize(true);
        // Set tooltip text for the tray icon
        trayIcon.setToolTip("");
        tray.add(trayIcon);

        trayIcon.displayMessage(APPLICATION_NAME, text, MessageType.INFO);
    }

    private class ButtonActionListener implements ActionListener {
        @Override
        public void actionPerformed(ActionEvent e) {
            button.setEnabled(false);
            new ButtonThread().start();
        }
    }

    private class ButtonThread extends Thread {
        @Override
        public void run() {
            try {
                while (true) {
                    final String spreadsheetId = id.getText();
                    final String range = sheet.getText() + "!" + col.getText() + row.getText();
                    String value = readCell(spreadsheetId, range);
                    if (value != null) {
                        currentText.setText(value);
                        if (value.compareTo(text.getText()) == 0) {
                            // two texts are equal
                            if (SystemTray.isSupported()) {
                                alert("'" + value + "' is detected on '" + range + "'!");
                            } else {
                                System.err.println("System tray not supported!");
                            }
                            button.setEnabled(true);
                            break;
                        }
                    }
                    Thread.sleep(1000);
                }
            } catch (Exception ex) {
                System.err.println(ex);
            }
        }
    }

    public GoogleSpreadsheetObserver(String title, int width, int height) {
        super(title);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(width, height);
        final GridBagLayout g = new GridBagLayout();
        setLayout(g);
        final GridBagConstraints c = new GridBagConstraints();
        c.gridx = GridBagConstraints.RELATIVE;
        c.gridy = GridBagConstraints.RELATIVE;
        c.gridwidth = 1;
        c.gridheight = 1;
        c.weightx = 1;
        c.weighty = 1;
        c.fill = GridBagConstraints.BOTH;
        c.insets = new Insets(1, 1, 1, 1);
        JLabel idLabel = new JLabel("Spreadsheet ID:");
        final JLabel sheetLabel = new JLabel("Sheet name:");
        final JLabel colLabel = new JLabel("Column:");
        final JLabel rowLabel = new JLabel("Row:");
        final JLabel textLabel = new JLabel("Text:");
        final JLabel lcurrentText = new JLabel("Current Text:");
        g.setConstraints(idLabel, c);
        c.gridwidth = GridBagConstraints.REMAINDER;
        g.setConstraints(id, c);
        c.gridwidth = 1;
        g.setConstraints(sheetLabel, c);
        c.gridwidth = GridBagConstraints.REMAINDER;
        g.setConstraints(sheet, c);
        c.gridwidth = 1;
        g.setConstraints(colLabel, c);
        c.gridwidth = GridBagConstraints.REMAINDER;
        g.setConstraints(col, c);
        c.gridwidth = 1;
        g.setConstraints(rowLabel, c);
        c.gridwidth = GridBagConstraints.REMAINDER;
        g.setConstraints(row, c);
        c.gridwidth = 1;
        g.setConstraints(textLabel, c);
        c.gridwidth = GridBagConstraints.REMAINDER;
        g.setConstraints(text, c);
        c.gridwidth = 1;
        g.setConstraints(lcurrentText, c);
        c.gridwidth = GridBagConstraints.REMAINDER;
        g.setConstraints(currentText, c);
        c.gridwidth = 2;
        g.setConstraints(button, c);
        add(idLabel);
        add(id);
        add(sheetLabel);
        add(sheet);
        add(colLabel);
        add(col);
        add(rowLabel);
        add(row);
        add(textLabel);
        add(text);
        add(lcurrentText);
        add(currentText);
        add(button);
        button.addActionListener(new ButtonActionListener());
        setVisible(true);
    }



    /**
     * Creates an authorized Credential object.
     *
     * @param HTTP_TRANSPORT The network HTTP Transport.
     * @return An authorized Credential object.
     * @throws IOException If the credentials.json file cannot be found.
     */
    private static Credential getCredentials(final NetHttpTransport HTTP_TRANSPORT)
            throws IOException {
        // Load client secrets.
        InputStream in = GoogleSpreadsheetObserver.class.getResourceAsStream(CREDENTIALS_FILE_PATH);
        if (in == null) {
            throw new FileNotFoundException("Resource not found: " + CREDENTIALS_FILE_PATH);
        }
        GoogleClientSecrets clientSecrets = GoogleClientSecrets.load(JSON_FACTORY, new InputStreamReader(in));

        // Build flow and trigger user authorization request.
        GoogleAuthorizationCodeFlow flow = new GoogleAuthorizationCodeFlow.Builder(
                HTTP_TRANSPORT, JSON_FACTORY, clientSecrets, SCOPES)
                .setDataStoreFactory(new FileDataStoreFactory(new java.io.File(TOKENS_DIRECTORY_PATH)))
                .setAccessType("offline")
                .build();
        LocalServerReceiver receiver = new LocalServerReceiver.Builder().setPort(8888).build();
        return new AuthorizationCodeInstalledApp(flow, receiver).authorize("user");
    }

    

    
}
