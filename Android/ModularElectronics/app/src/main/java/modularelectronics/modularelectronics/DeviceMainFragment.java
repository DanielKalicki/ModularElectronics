package modularelectronics.modularelectronics;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

public class DeviceMainFragment extends Fragment  {

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View V = inflater.inflate(R.layout.device_main_fragment, container, false);

        return V;
    }

    public void setText(String text){
        TextView textView = (TextView) (getView().findViewById(R.id.textField));
        textView.setText(text);
    }

    public void appendText(String text){
        TextView textView = (TextView) (getView().findViewById(R.id.textField));
        textView.append(text);
    }

}