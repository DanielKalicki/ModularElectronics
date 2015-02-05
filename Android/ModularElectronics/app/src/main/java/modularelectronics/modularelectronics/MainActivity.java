package modularelectronics.modularelectronics;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
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
import android.net.NetworkInfo;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.ExpandableListView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;


public class MainActivity extends Activity {

    private BluetoothAdapter mBluetoothAdapter;
    private boolean mScanning;
    private Handler mHandler;

    private BluetoothLeService mBluetoothLeService;

    private final static int REQUEST_ENABLE_BT = 1;
    private static final long SCAN_PERIOD=10_000;

    private ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics =
            new ArrayList<ArrayList<BluetoothGattCharacteristic>>();
    private BluetoothGattCharacteristic mNotifyCharacteristic;

    private String mDeviceAddress="78:A5:04:85:26:65";
    private boolean mConnected = false;

    TextView bleTerminal_text;
    TextView bleTerminal_output;

    private String modulesDescription="web\n";

    // Code to manage Service lifecycle.
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

    // Handles various events fired by the Service.
    // ACTION_GATT_CONNECTED: connected to a GATT server.
    // ACTION_GATT_DISCONNECTED: disconnected from a GATT server.
    // ACTION_GATT_SERVICES_DISCOVERED: discovered GATT services.
    // ACTION_DATA_AVAILABLE: received data from the device.  This can be a result of read
    //                        or notification operations.
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

    // Demonstrates how to iterate through the supported GATT Services/Characteristics.
    // In this sample, we populate the data structure that is bound to the ExpandableListView
    // on the UI.
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

    private void clearUI() {
        bleTerminal_text.setText("");
    }

    // If a given GATT characteristic is selected, check for supported features.  This sample
    // demonstrates 'Read' and 'Notify' features.  See
    // http://d.android.com/reference/android/bluetooth/BluetoothGatt.html for the complete
    // list of supported characteristic features.
    private final ExpandableListView.OnChildClickListener servicesListClickListner =
            new ExpandableListView.OnChildClickListener() {
                @Override
                public boolean onChildClick(ExpandableListView parent, View v, int groupPosition,
                                            int childPosition, long id) {
                    if (mGattCharacteristics != null) {
                        final BluetoothGattCharacteristic characteristic =
                                mGattCharacteristics.get(groupPosition).get(childPosition);
                        final int charaProp = characteristic.getProperties();
                        if ((charaProp | BluetoothGattCharacteristic.PROPERTY_READ) > 0) {
                            // If there is an active notification on a characteristic, clear
                            // it first so it doesn't update the data field on the user interface.
                            if (mNotifyCharacteristic != null) {
                                mBluetoothLeService.setCharacteristicNotification(
                                        mNotifyCharacteristic, false);
                                mNotifyCharacteristic = null;
                            }
                            mBluetoothLeService.readCharacteristic(characteristic);
                        }
                        if ((charaProp | BluetoothGattCharacteristic.PROPERTY_NOTIFY) > 0) {
                            mNotifyCharacteristic = characteristic;
                            mBluetoothLeService.setCharacteristicNotification(
                                    characteristic, true);
                        }
                        return true;
                    }
                    return false;
                }
            };



    @Override
    protected void onCreate(Bundle savedInstanceState) {

        Log.e("-","onCreate()");

        super.onCreate(savedInstanceState);
        getActionBar().setTitle("Modular electronics");
        mHandler=new Handler();

        setContentView(R.layout.activity_main);

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

        Intent gattServiceIntent = new Intent(this, BluetoothLeService.class);
        bindService(gattServiceIntent, mServiceConnection, Context.BIND_AUTO_CREATE);

        bleTerminal_text.append("onCreate()\n");

        new Thread(new Runnable(){
            public void run(){
                //TODO check if there is network connection here
                getModuleInformationFromHttp();
            }
        }).start();
    }

    public static final boolean CheckInternetConnection(Context context) {
        ConnectivityManager cm = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);

        if (cm.getActiveNetworkInfo() != null && cm.getActiveNetworkInfo().isAvailable() && cm.getActiveNetworkInfo().isConnected()) {
            return true;
        } else {
            return false;
        }
    }

    private void getModuleInformationFromHttp() {
        URLConnection feedUrl;

        if (CheckInternetConnection(MainActivity.this)==true){ //check internet connection
            Log.e("-","Internet ok!");
            try {
                // Create a URL for the desired page
                feedUrl = new URL("http://www.student.agh.edu.pl/dkalicki/modulesDescription.txt").openConnection();
                InputStream is = feedUrl.getInputStream();

                // Read all the text returned by the server
                BufferedReader in = new BufferedReader(new InputStreamReader(is,"UTF-8"));
                String str;
                while ((str = in.readLine()) != null) {
                    // str is one line of text; readLine() strips the newline character(s)
                    modulesDescription += str +"\n";
                }
                in.close();
                Log.e("-",modulesDescription);
                //TODO store this on the device
            } catch (MalformedURLException e) {
                Log.e("-","getModuleInformationFromHttp() - MalformedURLException");
            } catch (IOException e) {
                Log.e("-","getModuleInformationFromHttp() - IOException");
            }
        }
        else{
            Log.e("-","No internet");
        }
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

    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_CONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(BluetoothLeService.ACTION_DATA_AVAILABLE);
        return intentFilter;
    }

    private ArrayList<Double> temp = new ArrayList<>();
    private ArrayList<Double> light = new ArrayList<>();
    private ArrayList<Double> press = new ArrayList<>();

    private int dataCounter=0;
    private int dataErrCounter=0;
    private String prevLastOneData;

    private void getDataFromBle(String data) {

        //TODO try catch this function

        bleTerminal_text.setText("");
        bleTerminal_text.append(modulesDescription);
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
                bleTerminal_output.setText("");
            }
            for (int i=0;i<hexData.length;i++){
                bleTerminal_output.append(Integer.toString(Integer.parseInt(hexData[i],16))+" ");
            }

            prevLastOneData=lastOne;
        }
            /*String[] dataSplit = data.split("\n");
            for (int i=0;i<dataSplit.length-1;i++) {

                bleTerminal_text.append(dataSplit[i]);
                String[] stringValue = dataSplit[i].split("\\[",2);
                stringValue[0] = stringValue[0].replaceAll("[^\\d.]","");
                double value = 0;
                try{
                    value = Double.parseDouble(stringValue[0]);
                } catch (NumberFormatException e) {
                    value = 0;
                }

                if (dataSplit[i].charAt(1)==':') {
                    switch (dataSplit[i].charAt(0)) {
                        case 'T':
                            temp.add(value);
                            break;
                        case 'L':
                            light.add(value);
                            break;
                        case 'P':
                            press.add(value);
                            break;
                    }
                }

            }
        }

        try {
            bleTerminal_output.setText("");

            int arrayTempSize=temp.size();
            for(int i=arrayTempSize-1;i>=arrayTempSize-20;i--){
                bleTerminal_output.append(Double.toString(temp.get(i))+ "    ");
                bleTerminal_output.append(Double.toString(light.get(i))+ "    ");
                bleTerminal_output.append(Double.toString(press.get(i))+ "");
                bleTerminal_output.append("\n");
            }

        }
        catch (IndexOutOfBoundsException e) {
            //dont do anythin, we are still waiting for the tab to full up.
        }*/
    }

    private void printArrayData(ArrayList<Double> arr){
        int arrayTempSize=arr.size();
        for(int i=arrayTempSize-1;i>=arrayTempSize-10;i--){
            bleTerminal_output.append(Double.toString(arr.get(i))+ " ");
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

    private void scanLeDevice(final boolean enable) {
        if(enable){
            mHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    mScanning=false;
                    mBluetoothAdapter.startLeScan(mLeScanCallback);
                    invalidateOptionsMenu();
                }
            },SCAN_PERIOD);

            mScanning=true;
            mBluetoothAdapter.startLeScan(mLeScanCallback);

        } else {
            mScanning=false;
            mBluetoothAdapter.stopLeScan(mLeScanCallback);
        }
        invalidateOptionsMenu();
    }

    // Device scan callback.
    private BluetoothAdapter.LeScanCallback mLeScanCallback =
            new BluetoothAdapter.LeScanCallback() {

                @Override
                public void onLeScan(final BluetoothDevice device,final int rssi, byte[] scanRecord) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            bleTerminal_text.setText("Found: " + device.getName() + " " + Integer.toString(rssi) + "\n");
                            bleTerminal_text.append("Addr: " + device.getAddress() + " Uuids" + device.getUuids() + " Type:" + device.getType() + "\n");
                            if(device.getName().equals("DanK")){
                                bleTerminal_text.append("Bluetooth module found\n");
                                scanLeDevice(false);
                            }

                        }/**/
                    });
                }
            };

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
