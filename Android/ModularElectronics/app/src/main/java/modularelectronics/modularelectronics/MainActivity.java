package modularelectronics.modularelectronics;

import android.app.ActionBar;
import android.app.FragmentTransaction;
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
import android.support.v4.app.FragmentActivity;
import android.support.v4.view.ViewPager;
import android.util.Log;
import android.view.MenuItem;
import android.widget.Toast;

import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.GraphViewSeries;

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

import modularelectronics.modularelectronics.DeviceMainFragment.DeviceMainFragment;
import modularelectronics.modularelectronics.ModuleFragment.ModuleFragment;

public class MainActivity extends FragmentActivity implements ActionBar.TabListener{

    private BluetoothAdapter mBluetoothAdapter;
    private Handler mHandler;

    private BluetoothLeService mBluetoothLeService;

    private ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics =  new ArrayList<>();
    private BluetoothGattCharacteristic mWriteCharacteristic;

    private String mDeviceAddress="78:A5:04:3E:EA:33";//"78:A5:04:85:26:65";

    String   plotVariableName="BMP085 temp";

    ActionBar actionbar;
    ViewPager viewpager;
    FragmentPageAdapter ft;

    DeviceMainFragment deviceMainFragment;

    public class cModule{
        private Integer id;
        private String name;
        private Map<String, Double> rData;
        private ModuleFragment moduleFragment;

        public void add(Integer mId){
            id=mId;
        }
        public void addName(String mName){
            name=mName;
        }
        public void setModuleFragment(ModuleFragment mModuleFragment) {moduleFragment=mModuleFragment;}
    };


    Intent gattServiceIntent;

    private String modulesDescriptionFile="";
    private String modulesDescription_FileName="moduleDescription.xml";

    //-------------------------------------------
    //      Code to manage Service lifecycle.
    //-------------------------------------------
    private final ServiceConnection mServiceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            //bleTerminal_text.append("onServiceConnected()\n");
            mBluetoothLeService = ((BluetoothLeService.LocalBinder) service).getService();
            if (!mBluetoothLeService.initialize()) {
                //bleTerminal_text.append("- Unable to initialize Bluetooth\n");
                finish();
            }
            // Automatically connects to the device upon successful start-up initialization.
            //bleTerminal_text.append("Bluetooth init ok, trying to connect\n");
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
    private void initializeBluetoothAdapter() {
        // Use this check to determine whether BLE is supported on the device.  Then you can
        // selectively disable BLE-related features.
        if(!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)){
            Toast.makeText(this,R.string.ble_not_supported,Toast.LENGTH_SHORT).show();
            finish();
        }

        final BluetoothManager bluetoothManager =
                (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = bluetoothManager.getAdapter();

        // Ensures Bluetooth is available on the device and it is enabled. If not,
        // displays a dialog requesting user permission to enable Bluetooth.
        if (mBluetoothAdapter == null || !mBluetoothAdapter.isEnabled()) {
            Toast.makeText(this, R.string.error_bluetooth_not_supported, Toast.LENGTH_SHORT).show();
            finish();
        }

        gattServiceIntent = new Intent(this, BluetoothLeService.class);
        bindService(gattServiceIntent, mServiceConnection, Context.BIND_AUTO_CREATE);
    }
    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if(deviceMainFragment==null){
                deviceMainFragment = (DeviceMainFragment)getSupportFragmentManager().findFragmentByTag("android:switcher:" + R.id.pager + ":" + 0);
            }
            switch (action) {
                case BluetoothLeService.ACTION_GATT_CONNECTED:
                    updateConnectionState("connected");
                    deviceMainFragment.setBleDeviceIdField(mDeviceAddress);
                    invalidateOptionsMenu();
                    break;
                case BluetoothLeService.ACTION_GATT_DISCONNECTED:
                    updateConnectionState("unconnected");
                    //TODO automaticly try to connect again.
                    //bindService(gattServiceIntent, mServiceConnection, Context.BIND_AUTO_CREATE);
                    deviceMainFragment.noBleDeviceFound();
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
        //bleTerminal_text.append("displayGattServices()\n");
        if (gattServices == null) return;
        String uuid;
        ArrayList<HashMap<String, String>> gattServiceData = new ArrayList<HashMap<String, String>>();
        ArrayList<ArrayList<HashMap<String, String>>> gattCharacteristicData
                = new ArrayList<>();
        mGattCharacteristics = new ArrayList<>();

        //bleTerminal_text.setText("");

        // Loops through available GATT Services.
        for (BluetoothGattService gattService : gattServices) {
            HashMap<String, String> currentServiceData = new HashMap<>();
            uuid = gattService.getUuid().toString();
            //bleTerminal_output.append("(S): "+uuid+"\n");
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
                //bleTerminal_output.append("\t\t\t(C): "+uuid+"\n");
                gattCharacteristicGroupData.add(currentCharaData);

                if(uuid.equals("0000ffe1-0000-1000-8000-00805f9b34fb")) {
                    //bleTerminal_output.append("Service found\n");
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
        //bleTerminal_text.setText("");
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
    public static boolean CheckInternetConnection(Context context) {
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

    ArrayList<cModule> detectedModules = new ArrayList<>(); //ID of modules in the device

    private void getDataFromBle(String data) {

        //TODO try catch this function
        deviceMainFragment.setFrameCounter("data frames ok:"+Integer.toString(dataCounter)+" err:"+Integer.toString(dataErrCounter)+"\n");

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
                    /*if (d.get(1) != 68 ) {
                        deviceMainFragment.setReceiveDataField_set("");
                        parseReceiveModuleData(d);
                    }*/
                    if (d.get(d.size()-1)==124){ //last received element is 124
                        for (int i=0;i<d.size()-2;i++){
                            if (d.get(i)==124 && d.get(i+1)==d.size()-2) {//check if the length of the frame is correct according to d[2]
                                deviceMainFragment.setReceiveDataField_set("");
                                parseReceiveModuleData(d,i); //d.subset(i,end)
                                break;
                            }
                        }
                    }
                }catch(IndexOutOfBoundsException e){}

                d.clear();
            }
            for (int i=0;i<hexData.length;i++){
                Integer value = Integer.parseInt(hexData[i], 16);
                d.add(value);
                deviceMainFragment.setReceiveDataField_append(Integer.toString(value)+" ");
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
    private void parseReceiveModuleData(ArrayList<Integer> d, Integer startIndex) {
        //Container with variables names and values
        Map<String,Double> rData = new HashMap<>();

        //initialize MathEval script
        MathEval math=new MathEval();

        //xml parsing
        Document doc = parseXML(modulesDescriptionFile);
        try {
            doc.getDocumentElement().normalize();
        }
        catch (NullPointerException ignored) {return;}
        //Log.e("-","Root element :" + doc.getDocumentElement().getNodeName());

        //Check module id and add it to list of modules if it is not there already
        Integer moduleId = d.get(2+startIndex)*256*256+d.get(3+startIndex)*256+d.get(4+startIndex);
        Integer module_index = 0;
        boolean moduleAlreadyDetected = false;
        //TODO remove module if it wasnt detected for 30 seconds
        try {
            for (int i = 0; i < detectedModules.size(); i++) { //check if this module was detected already.
                if (moduleId.compareTo(detectedModules.get(i).id)==0) {
                    moduleAlreadyDetected = true;
                    module_index=i;
                    break;
                }
            }
        } catch (NullPointerException ignored) {}
        if (!moduleAlreadyDetected){
            detectedModules.add(new cModule());
            module_index=detectedModules.size()-1;
            detectedModules.get(module_index).add(moduleId);
        }

        //update device main fragment for this module with received data frame
        String frameData="";
        for (int i=startIndex;i<d.size();i++){
            frameData += String.format("%03d\t",d.get(i));
        }
        deviceMainFragment.updateModuleText(moduleId, frameData);
        if(detectedModules.get(module_index).moduleFragment==null){
            try{ModuleFragment moduleFragmentBuff = (ModuleFragment)getSupportFragmentManager().findFragmentByTag("android:switcher:" + R.id.pager + ":" + Integer.toString(module_index + 1));
                detectedModules.get(module_index).setModuleFragment(moduleFragmentBuff);
            }catch (NullPointerException ignored){}
        }

        //xml read tags
        NodeList nModuleList = doc.getElementsByTagName("Module");
        for (int i = 0; i < nModuleList.getLength(); i++) {
            //--Module---
            Node nModule = nModuleList.item(i);
            if (nModule.getNodeType() == Node.ELEMENT_NODE) {

                Element eModuleDesc = (Element) nModule;

                //Module name
                if(!moduleAlreadyDetected){
                    if (moduleId.compareTo(Integer.parseInt(eModuleDesc.getAttribute("id")))==0) {
                        String nameBuff = eModuleDesc.getAttribute("name");
                        detectedModules.get(module_index).addName(nameBuff);
                        deviceMainFragment.addNewModuleToList(moduleId,nameBuff + " [" + Integer.toString(moduleId) + "]");

                        int countNumber = ft.getRealCount();
                        ft.setCount(countNumber + 1);
                        actionbar.addTab(getActionBar().newTab().setText(nameBuff).setTabListener(this));
                    }
                }

                //--Variable--
                NodeList nVariableList = eModuleDesc.getElementsByTagName("Variable");
                for (int ii = 0; ii < nVariableList.getLength(); ii++) {

                    Node nVariable = nVariableList.item(ii);

                    if (nVariable.getNodeType() == Node.ELEMENT_NODE) {

                        Element eVariableDesc = (Element) nVariable;

                        Double value;
                        String varEquation = eVariableDesc.getAttribute("equation");
                        String varName = eVariableDesc.getAttribute("name");
                        String equation = varEquation;
                        Pattern pattern = Pattern.compile("d\\[[0-9]+\\]");
                        Matcher matcher = pattern.matcher(varEquation);
                        while (matcher.find())
                        {
                            String matchedStr = matcher.group(0);
                            String numbs= matchedStr.replaceAll("[^0-9]", "");
                            Integer numb = Integer.parseInt(numbs)+1+startIndex;   //plus one because the first character is start indicator.
                            try {
                                String dVal = Integer.toString(d.get(numb));
                                equation = equation.replace(matchedStr, dVal);
                            }
                            catch (IndexOutOfBoundsException ignored){};
                        }

                        try {
                            value = math.evaluate(equation);
                            if (varName.compareTo(plotVariableName)==0){
                                //Log.e("-", eVariableDesc.getAttribute("equation")+"->"+equation+" = "+value);
                            }
                        }
                        catch(ArithmeticException e) {
                            value = 0.0d/0.0;
                        }
                        rData.put(varName,value);
                        try {
                            if (moduleId.compareTo(Integer.parseInt(eModuleDesc.getAttribute("id")))==0) {
                                detectedModules.get(module_index).moduleFragment.setVariable(varName, Double.toString(value));
                            }
                        }catch (NullPointerException ignored){}
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
        /*graphDataPoints = new GraphViewSeries(new GraphViewData[] {
                new GraphViewData(0, 0.0d)
        });
        graphView = new LineGraphView( this, plotVariableName );
        graphDataPointsSize = 1;
        graphDataPointX=0;

        graphView.addSeries(graphDataPoints);
        graphView.setScrollable(true);
        graphView.setViewPort(0,graphDataPointsSize);
        //LinearLayout layout = (LinearLayout) findViewById(R.id.layout);
        //layout.addView(graphView);
        */
    }
    protected void graphAddPoint(double x, double y){
        /*if(graphDataPointX < x) { //x must be greater than previous x
            graphDataPointsSize++;
            graphDataPointX = x;
            if(graphDataPointsSize-graphStartPoint>1000){
                graphStartPoint++;
            }
            graphView.setViewPort(graphStartPoint, graphDataPointsSize-graphStartPoint);
            graphDataPoints.appendData(new GraphViewData(x, y), true, 1000);
            graphView.redrawAll();
        }*/
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

        viewpager = (ViewPager)findViewById(R.id.pager);

        ft = new FragmentPageAdapter(getSupportFragmentManager());
        ft.setCount(1);
        viewpager.setAdapter(ft);
        viewpager.setOffscreenPageLimit(100);
        actionbar= getActionBar();
        actionbar.setNavigationMode(ActionBar.NAVIGATION_MODE_TABS);
        actionbar.addTab(actionbar.newTab().setText("Device").setTabListener(this));

        viewpager.setOnPageChangeListener(new ViewPager.OnPageChangeListener() {
            @Override
            public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels) {

            }

            @Override
            public void onPageSelected(int position) {
                if (position < ft.getRealCount()) {
                    actionbar.setSelectedNavigationItem(position);
                }
                else {
                    viewpager.setCurrentItem(ft.getRealCount()-1);
                }
            }

            @Override
            public void onPageScrollStateChanged(int state) {

            }
        });

        graphInit();

        // Initializes Bluetooth adapter.
        initializeBluetoothAdapter();

        new Thread(new Runnable(){
            public void run(){
                getModulesInformation();
            }
        }).start();

    }
    @Override
    protected void onStart(){
        super.onStart();
    }
    @Override
    protected void onResume() {
        super.onResume();
        registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
        if (mBluetoothLeService != null) {
            final boolean result = mBluetoothLeService.connect(mDeviceAddress);
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

    //Tab functions
    @Override
    public void onTabSelected(ActionBar.Tab tab, FragmentTransaction ft) {
        viewpager.setCurrentItem(tab.getPosition());
    }
    @Override
    public void onTabUnselected(ActionBar.Tab tab, FragmentTransaction ft) {

    }
    @Override
    public void onTabReselected(ActionBar.Tab tab, FragmentTransaction ft) {

    }
}
