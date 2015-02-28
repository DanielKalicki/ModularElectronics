package modularelectronics.modularelectronics;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.GraphViewSeries;
import com.jjoe64.graphview.LineGraphView;
import com.jjoe64.graphview.GraphView.GraphViewData;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.StringReader;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

public class MainActivity extends Activity {

    private BluetoothAdapter mBluetoothAdapter;
    private Handler mHandler;

    private BluetoothLeService mBluetoothLeService;

    private ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics =  new ArrayList<>();
    private BluetoothGattCharacteristic mWriteCharacteristic;

    private String mDeviceAddress="78:A5:04:85:26:65";
    private boolean mConnected = false;

    TextView bleTerminal_text;
    TextView bleTerminal_output;

    Button   mPlotSelectButton;
    EditText mPlotVariableName;
    String   plotVariableName="BMP085 temp";

    Intent gattServiceIntent;

    private String modulesDescriptionFile="";
    private String modulesDescription_FileName="moduleDescription.xml";

    //-------------------------------------------
    //      Code to manage Service lifecycle.
    //-------------------------------------------
    private final ServiceConnection mServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            bleTerminal_text.append("onServiceConnected()\n");
            mBluetoothLeService = ((BluetoothLeService.LocalBinder) service).getService();
            if (!mBluetoothLeService.initialize()) {
                bleTerminal_text.append("- Unable to initialize Bluetooth\n");
                finish();
            }
            // Automatically connects to the device upon successful start-up initialization.
            bleTerminal_text.append("Bluetooth init ok, trying to connect\n");
            mBluetoothLeService.connect(mDeviceAddress);
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mBluetoothLeService = null;
        }
    };

    //-------------------------------------------
    //             Ble functions
    //-------------------------------------------
    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            switch (action) {
                case BluetoothLeService.ACTION_GATT_CONNECTED:
                    mConnected = true;
                    updateConnectionState("connected");
                    invalidateOptionsMenu();
                    break;
                case BluetoothLeService.ACTION_GATT_DISCONNECTED:
                    mConnected = false;
                    updateConnectionState("unconnected");
                    //TODO automaticly try to connect again.
                    //bindService(gattServiceIntent, mServiceConnection, Context.BIND_AUTO_CREATE);
                    invalidateOptionsMenu();
                    clearUI();
                    break;
                case BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED:
                    // Show all the supported services and characteristics on the user interface.
                    displayGattServices(mBluetoothLeService.getSupportedGattServices());
                    break;
                case BluetoothLeService.ACTION_DATA_AVAILABLE:
                    getDataFromBle(intent.getStringExtra(BluetoothLeService.EXTRA_DATA));
                    break;
            }
        }
    };

    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_CONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(BluetoothLeService.ACTION_DATA_AVAILABLE);
        return intentFilter;
    }

    private void displayGattServices(List<BluetoothGattService> gattServices) {
        bleTerminal_text.append("displayGattServices()\n");
        if (gattServices == null) return;
        String uuid;
        ArrayList<HashMap<String, String>> gattServiceData = new ArrayList<HashMap<String, String>>();
        ArrayList<ArrayList<HashMap<String, String>>> gattCharacteristicData
                = new ArrayList<>();
        mGattCharacteristics = new ArrayList<>();

        bleTerminal_text.setText("");

        // Loops through available GATT Services.
        for (BluetoothGattService gattService : gattServices) {
            HashMap<String, String> currentServiceData = new HashMap<>();
            uuid = gattService.getUuid().toString();
            bleTerminal_output.append("(S): "+uuid+"\n");
            gattServiceData.add(currentServiceData);

            ArrayList<HashMap<String, String>> gattCharacteristicGroupData =
                    new ArrayList<>();
            List<BluetoothGattCharacteristic> gattCharacteristics =
                    gattService.getCharacteristics();
            ArrayList<BluetoothGattCharacteristic> charas =
                    new ArrayList<>();

            // Loops through available Characteristics.
            for (BluetoothGattCharacteristic gattCharacteristic : gattCharacteristics) {
                charas.add(gattCharacteristic);
                HashMap<String, String> currentCharaData = new HashMap<>();
                uuid = gattCharacteristic.getUuid().toString();
                bleTerminal_output.append("\t\t\t(C): "+uuid+"\n");
                gattCharacteristicGroupData.add(currentCharaData);

                if(uuid.equals("0000ffe1-0000-1000-8000-00805f9b34fb")) {
                    bleTerminal_output.append("Service found\n");
                    mBluetoothLeService.setCharacteristicNotification(gattCharacteristic, true);
                    mWriteCharacteristic = gattCharacteristic;
                }
            }
            mGattCharacteristics.add(charas);
            gattCharacteristicData.add(gattCharacteristicGroupData);
        }
    }

    private void updateConnectionState(final String text) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                getActionBar().setTitle(text);
            }
        });
    }

    private void writeCharacteristic(BluetoothGattCharacteristic characteristic) {
        if (mBluetoothLeService != null) {
            mBluetoothLeService.writeCharacteristic(characteristic);
        }
    }

    private void clearUI() {
        bleTerminal_text.setText("");
    }

    //-------------------------------------------
    //       Get module description file
    //-------------------------------------------
    private void getModulesInformation() {
        if (CheckInternetConnection(MainActivity.this)==true){ //check internet connection
            try {
                // Create a URL for the desired page
                URLConnection feedUrl;
                Log.e("-","http://www.student.agh.edu.pl/dkalicki/"+modulesDescription_FileName);
                //TODO change file name to modulesDescription_FileName
                feedUrl = new URL("http://www.student.agh.edu.pl/dkalicki/"+modulesDescription_FileName).openConnection();

                InputStream is = feedUrl.getInputStream();

                // Read all the text returned by the server
                BufferedReader in = new BufferedReader(new InputStreamReader(is,"UTF-8"));
                String str;
                while ((str = in.readLine()) != null) {
                    // str is one line of text; readLine() strips the newline character(s)
                    modulesDescriptionFile += str +"\n";
                }
                in.close();
                Log.e("-", "Module desc file read from INTERNET");
                //Log.e("-",modulesDescriptionFile);

                storeDescriptionFile(modulesDescriptionFile);

            } catch (MalformedURLException e) {
                Log.e("-","getModuleInformation() - MalformedURLException");
                return;
            } catch (IOException e) {
                Log.e("-","getModuleInformation() - IOException");
                return;
            }
        }
        else {
            try {
                FileInputStream fin = openFileInput(modulesDescription_FileName);
                int c;
                String temp = "";
                while ((c = fin.read()) != -1) {
                    temp = temp + Character.toString((char) c);
                }
                modulesDescriptionFile = temp;
                Log.e("-", "Module desc file read from internal storage:");
                //Log.e("-", "------"+modulesDescriptionFile);

            } catch (Exception e) {
                Log.e("-","getModuleInformation() - No internet IOException");
                return;
            }
        }
        parseXML(modulesDescriptionFile);
    }

    public static final boolean CheckInternetConnection(Context context) {
        ConnectivityManager cm = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);

        if (cm.getActiveNetworkInfo() != null && cm.getActiveNetworkInfo().isAvailable() && cm.getActiveNetworkInfo().isConnected()) {
            return true;
        } else {
            return false;
        }
    }
    private void storeDescriptionFile(String modulesDescriptionFile) {
        FileOutputStream fOut = null;
        try {
            fOut = openFileOutput(modulesDescription_FileName,MODE_PRIVATE);
            if (fOut != null) {
                try {
                    fOut.write(modulesDescriptionFile.getBytes());
                    fOut.close();
                } catch (IOException e1) {
                    Log.e("-", "IOException file write");
                }
            }

        } catch (FileNotFoundException e) {
            Log.e("-","Write file not found");
        }
    }

    private Document parseXML(String xml){
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        DocumentBuilder builder = null;
        try {
            builder = factory.newDocumentBuilder();
        } catch (ParserConfigurationException e) {
            e.printStackTrace();
            return null;
        }
        InputSource is = new InputSource(new StringReader(xml));
        Document doc;
        try {
            doc = builder.parse(is);
        } catch (SAXException e) {
            e.printStackTrace();
            return null;
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }

        return doc;
    }

    //-------------------------------------------
    //          Parse received Ble data
    //-------------------------------------------
    private int dataCounter=0;
    private int dataErrCounter=0;
    private String prevLastOneData;
    ArrayList<Integer> d = new ArrayList<>();

    private void getDataFromBle(String data) {

        //TODO try catch this function

        bleTerminal_text.setText("");
        //bleTerminal_text.append(modulesDescriptionFile);
        bleTerminal_text.append("data frames ok:"+Integer.toString(dataCounter)+" err:"+Integer.toString(dataErrCounter)+"\n");

        if (data != null) {

            String[] dataSplit = data.split("\n");
            String lastOne = dataSplit[dataSplit.length - 1];
            String[] hexData=lastOne.split(" ");
            if (lastOne.charAt(0)== '7') {      //7 is the first char of 124 in hex
                if(prevLastOneData!=null) {
                    String[] prevHexData = prevLastOneData.split(" ");
                    if (Integer.parseInt(prevHexData[prevHexData.length - 1], 16) != 124) {
                        dataErrCounter++;
                    }
                }
                dataCounter++;
                try {
                    if (d.get(1) != 68 ) {
                        bleTerminal_output.setText("");
                        parseReceiveModuleData(d);
                    }
                }catch(IndexOutOfBoundsException e){}


                d.clear();
            }
            for (int i=0;i<hexData.length;i++){
                Integer value = Integer.parseInt(hexData[i], 16);

                bleTerminal_output.append(Integer.toString(value) + " ");

                d.add(value);
            }

            prevLastOneData=lastOne;

            /*try {
                String str = Character.toString((char)16)+"W"+Character.toString((char)4)+Character.toString((char)(dataCounter%128));
                byte[] strBytes = str.getBytes();
                byte[] bytes = MainActivity.this.mWriteCharacteristic.getValue();

                if (bytes == null) {
                    // maybe just write a byte into GATT
                    Log.e("-", "Cannot get Values from mWriteCharacteristic.");
                    return;
                } else if (bytes.length <= strBytes.length) {
                    for (int i = 0; i < bytes.length; i++) {
                        bytes[i] = strBytes[i];
                    }
                } else {
                    for (int i = 0; i < strBytes.length; i++) {
                        bytes[i] = strBytes[i];
                    }
                }

                MainActivity.this.mWriteCharacteristic.setValue(bytes);
                MainActivity.this.writeCharacteristic(MainActivity.this.mWriteCharacteristic);
            }
            catch (NullPointerException e){
                Log.e("-","mWriteCharacteristic NullPointerException");
            }*/
        }
    }

    private void parseReceiveModuleData(ArrayList<Integer> d) {
        Map<String,Double> rData = new HashMap<>();

        MathEval math=new MathEval();

        Document doc = parseXML(modulesDescriptionFile);

        doc.getDocumentElement().normalize();
        //Log.e("-","Root element :" + doc.getDocumentElement().getNodeName());

        NodeList nModuleList = doc.getElementsByTagName("Module");
        for (int i = 0; i < nModuleList.getLength(); i++) {
            //--Module---
            Node nModule = nModuleList.item(i);
            if (nModule.getNodeType() == Node.ELEMENT_NODE) {

                Element eModuleDesc = (Element) nModule;

                //--Variable--
                NodeList nVariableList = eModuleDesc.getElementsByTagName("Variable");
                for (int ii = 0; ii < nVariableList.getLength(); ii++) {

                    Node nVariable = nVariableList.item(ii);

                    if (nVariable.getNodeType() == Node.ELEMENT_NODE) {

                        Element eVariableDesc = (Element) nVariable;

                        Double value=0.0;
                        String varEquation = eVariableDesc.getAttribute("equation");
                        String equation = varEquation;
                        Pattern pattern = Pattern.compile("d\\[[0-9]+\\]");
                        Matcher matcher = pattern.matcher(varEquation);
                        while (matcher.find())
                        {
                            String matchedStr = matcher.group(0);
                            String numbs= matchedStr.replaceAll("[^0-9]", "");
                            Integer numb = Integer.parseInt(numbs)+1;   //plus one because the first character is start indicator.
                            try {
                                String dVal = Integer.toString(d.get(numb));
                                equation = equation.replace(matchedStr, dVal);
                            }
                            catch (IndexOutOfBoundsException e){};
                        }

                        try {
                            value = math.evaluate(equation);
                        }
                        catch(ArithmeticException e) {
                            value = 0.0d/0.0;
                        }
                        rData.put(eVariableDesc.getAttribute("name"),value);
                    }
                }
            }
        }
        //Log.e("-", rData.get(plotVariableName).toString());
        try {
            graphAddPoint(graphDataPointX + 1, rData.get(plotVariableName));
        }
        catch (NullPointerException e) {
            graphAddPoint(graphDataPointX + 1, 0);
        }
    }

    //-------------------------------------------
    //              Graph functions
    //-------------------------------------------
    GraphView graphView;
    GraphViewSeries graphDataPoints;
    int graphDataPointsSize;
    int graphStartPoint=0;
    double graphDataPointX;

    protected void graphInit(){
        // initialize graph series data
        graphDataPoints = new GraphViewSeries(new GraphViewData[] {
                new GraphViewData(0, 0.0d)
        });
        graphView = new LineGraphView( this, plotVariableName );
        graphDataPointsSize = 1;
        graphDataPointX=0;

        graphView.addSeries(graphDataPoints);
        graphView.setScrollable(true);
        graphView.setViewPort(0,graphDataPointsSize);
        LinearLayout layout = (LinearLayout) findViewById(R.id.layout);
        layout.addView(graphView);
    }

    protected void graphAddPoint(double x, double y){
        if(graphDataPointX < x) { //x must be greater than previous x
            graphDataPointsSize++;
            graphDataPointX = x;
            if(graphDataPointsSize-graphStartPoint>1000){
                graphStartPoint++;
            }
            graphView.setViewPort(graphStartPoint, graphDataPointsSize-graphStartPoint);
            graphDataPoints.appendData(new GraphViewData(x, y), true, 1000);
            graphView.redrawAll();
        }
    }

    //-------------------------------------------
    //      Application life time functions
    //-------------------------------------------
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.e("-","onCreate()");

        super.onCreate(savedInstanceState);
        getActionBar().setTitle("Modular electronics");
        mHandler=new Handler();

        setContentView(R.layout.activity_main);

        mPlotSelectButton = (Button)findViewById(R.id.plotSelectButton);
        mPlotVariableName   = (EditText)findViewById(R.id.plotVariableName);

        mPlotSelectButton.setOnClickListener(
            new View.OnClickListener()
            {
                public void onClick(View view)
                {
                    graphView.setTitle(mPlotVariableName.getText().toString());
                    plotVariableName=mPlotVariableName.getText().toString();
                    graphStartPoint=graphDataPointsSize+1;
                    graphView.redrawAll();
                }
            });

        graphInit();

        // Use this check to determine whether BLE is supported on the device.  Then you can
        // selectively disable BLE-related features.
        if(!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)){
            Toast.makeText(this,R.string.ble_not_supported,Toast.LENGTH_SHORT).show();
            finish();
        }

        // Initializes Bluetooth adapter.
        final BluetoothManager bluetoothManager =
                (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = bluetoothManager.getAdapter();

        // Ensures Bluetooth is available on the device and it is enabled. If not,
        // displays a dialog requesting user permission to enable Bluetooth.
        if (mBluetoothAdapter == null || !mBluetoothAdapter.isEnabled()) {
            Toast.makeText(this, R.string.error_bluetooth_not_supported, Toast.LENGTH_SHORT).show();
            finish();
        }

        bleTerminal_text = (TextView)findViewById(R.id.ble_terminal_text);
        bleTerminal_text.append("Test\n");

        bleTerminal_output=(TextView)findViewById(R.id.BluetoothTerminal_output);

        gattServiceIntent = new Intent(this, BluetoothLeService.class);
        bindService(gattServiceIntent, mServiceConnection, Context.BIND_AUTO_CREATE);

        bleTerminal_text.append("onCreate()\n");

        new Thread(new Runnable(){
            public void run(){
                getModulesInformation();
            }
        }).start();

    }
    @Override
    protected void onResume() {
        super.onResume();
        registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
        if (mBluetoothLeService != null) {
            final boolean result = mBluetoothLeService.connect(mDeviceAddress);
            bleTerminal_text.append("- Connect request result=" + result);
        }
    }
    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(mGattUpdateReceiver);
    }
    @Override
    protected void onDestroy() {
        super.onDestroy();
        unbindService(mServiceConnection);
        mBluetoothLeService = null;
    }

    //Menu functions
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
