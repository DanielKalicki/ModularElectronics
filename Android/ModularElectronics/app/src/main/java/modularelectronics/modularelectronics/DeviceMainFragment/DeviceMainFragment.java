package modularelectronics.modularelectronics.DeviceMainFragment;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.ListFragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.Arrays;

import modularelectronics.modularelectronics.R;

public class DeviceMainFragment extends ListFragment {

    private ArrayAdapter<String> listAdapter ;
    ArrayList<String> modulesList;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View V = inflater.inflate(R.layout.device_main_fragment, container, false);

        ListView moduleListView = (ListView) V.findViewById(android.R.id.list);
        // Create and populate a List of planet names.
        String[] modules = new String[] { "Mercury", "Venus", "Earth", "Mars",
                "Jupiter", "Saturn", "Uranus", "Neptune"};
        modulesList = new ArrayList<String>();
        modulesList.addAll( Arrays.asList(modules) );

        listAdapter = new ArrayAdapter<String>(getActivity(),R.layout.detected_modules_row, modulesList);

        setListAdapter(listAdapter);

        addNewModuleToList("Pluto ????");

        return V;
    }

    public void addNewModuleToList(String text){
        modulesList.add(text);
        listAdapter.notifyDataSetChanged();
    }

    public void noBleDeviceFound(){
        TextView textView = (TextView) (getView().findViewById(R.id.tvBleDeviceId));
        textView.setText("No bluetooth device present");
    }

    public void setBleDeviceIdField(String text){
        TextView textView = (TextView) (getView().findViewById(R.id.tvBleDeviceId));
        textView.setText("Bluetooth module id: "+text);
    }

    public void setFrameCounter(String text){
        TextView textView = (TextView) (getView().findViewById(R.id.tvFrameCounter));
        textView.setText(text);
    }

    public void setReceiveDataField(String text){
        TextView textView = (TextView) (getView().findViewById(R.id.tvReceiveData));
        textView.setText(text);
    }
}