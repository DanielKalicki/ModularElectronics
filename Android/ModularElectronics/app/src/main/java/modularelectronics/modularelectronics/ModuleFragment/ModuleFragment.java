package modularelectronics.modularelectronics.ModuleFragment;

import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.support.v4.app.ListFragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import java.util.ArrayList;
import java.util.List;

import modularelectronics.modularelectronics.R;

public class ModuleFragment extends ListFragment {
    private List<ListViewItem> mItems;
    ListViewModuleInfoAdapter listAdapter;
    View V;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        V = inflater.inflate(R.layout.module_fragment, container, false);

        mItems = new ArrayList<ListViewItem>();

        listAdapter = new ListViewModuleInfoAdapter(getActivity(), mItems);
        setListAdapter(listAdapter);
        return V;
    }

    public void addVariable(String varName, String varText){
        Drawable icon;
        if (varName.equals("Humidity"))         icon = getResources().getDrawable(R.drawable.hum);
        else if (varName.equals("Temperature")) icon = getResources().getDrawable(R.drawable.temp);
        else if (varName.equals("Ambient light")) icon = getResources().getDrawable(R.drawable.light);
        else if (varName.equals("Pressure")) icon = getResources().getDrawable(R.drawable.baro);
        else if (varName.equals("BMP085 temp")) icon = getResources().getDrawable(R.drawable.temp);
        else if (varName.equals("Compass X")) icon = getResources().getDrawable(R.drawable.comp_x);
        else if (varName.equals("Compass Y")) icon = getResources().getDrawable(R.drawable.comp_y);
        else if (varName.equals("Compass Z")) icon = getResources().getDrawable(R.drawable.comp_z);
        else if (varName.equals("Accel X")) icon = getResources().getDrawable(R.drawable.accel_x);
        else if (varName.equals("Accel Y")) icon = getResources().getDrawable(R.drawable.accel_y);
        else if (varName.equals("Accel Z")) icon = getResources().getDrawable(R.drawable.accel_z);
        else if (varName.equals("Gyro X")) icon = getResources().getDrawable(R.drawable.gyro_x);
        else if (varName.equals("Gyro Y")) icon = getResources().getDrawable(R.drawable.gyro_y);
        else if (varName.equals("Gyro Z")) icon = getResources().getDrawable(R.drawable.gyro_z);

        else if (varName.equals("Battery voltage")) icon = getResources().getDrawable(R.drawable.bat_v);
        else if (varName.equals("Battery temperature")) icon = getResources().getDrawable(R.drawable.bat_temp);
        else if (varName.equals("Battery charge")) icon = getResources().getDrawable(R.drawable.bat_uah);
        else if (varName.equals("Battery discharge rate")) icon = getResources().getDrawable(R.drawable.bat_disch_rate);
        else icon =getResources().getDrawable(R.drawable.ic_launcher);

        mItems.add(new ListViewItem(icon,varName,varText));
    }

    public void setVariable(String varName, String valueText){
        boolean varFound=false;
        for (int i=0;i<mItems.size();i++){
            if(mItems.get(i).title.compareTo(varName)==0){
                mItems.get(i).description=valueText;
                varFound=true;
                break;
            }
        }
        if(varFound==false){
            addVariable(varName,valueText);
        }
        listAdapter.notifyDataSetChanged();
    }
}

