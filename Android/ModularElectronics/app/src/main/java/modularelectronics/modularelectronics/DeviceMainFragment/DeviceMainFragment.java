package modularelectronics.modularelectronics.DeviceMainFragment;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.ExpandableListView;
import android.widget.TextView;

import java.util.ArrayList;

import modularelectronics.modularelectronics.R;

public class DeviceMainFragment extends Fragment { //ListFragment

    //private ArrayAdapter<String> listAdapter ;
    ArrayList<String> modulesList;

    private ExpandableListAdapter listAdapter;
    View V;
    ExpandableListView lv;
    //private String[] groups;
    private ArrayList<String> groups;
    private ArrayList<Integer> groups_Integer;
    private ArrayList<Object> children;

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        V = inflater.inflate(R.layout.device_main_fragment, container, false);

        modulesList = new ArrayList<>();
        //listAdapter = new ArrayAdapter<>(getActivity(),R.layout.detected_modules_row, modulesList);

        //setListAdapter(listAdapter);

        Log.e("-","Fragment creted");

        return V;
    }
    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        groups = new ArrayList<>();
        groups_Integer = new ArrayList<>();
        children = new ArrayList<>();

        lv = (ExpandableListView) view.findViewById(R.id.expListView);
        listAdapter = new ExpandableListAdapter(groups, children);
        lv.setAdapter(listAdapter);
        lv.setGroupIndicator(null);

        Log.e("-","Fragment View creted");
    }

    public class ExpandableListAdapter extends BaseExpandableListAdapter {

        private final LayoutInflater inf;
        private ArrayList<String> groups;
        private ArrayList<Object> children;

        public ExpandableListAdapter(ArrayList<String> groups, ArrayList<Object> children) {
            this.groups = groups;
            this.children = children;
            inf = LayoutInflater.from(getActivity());
        }

        @Override
        public int getGroupCount() {
            return groups.size();
        }
        @Override
        public int getChildrenCount(int groupPosition) {
            return((ArrayList<String>)children.get(groupPosition)).size();
        }
        @Override
        public Object getGroup(int groupPosition) {
            return groups.get(groupPosition);
        }
        @Override
        public Object getChild(int groupPosition, int childPosition) {
            return ((ArrayList<String>)children.get(groupPosition)).get(childPosition);
        }
        @Override
        public long getGroupId(int groupPosition) {
            return groupPosition;
        }
        @Override
        public long getChildId(int groupPosition, int childPosition) {
            return childPosition;
        }
        @Override
        public boolean hasStableIds() {
            return true;
        }
        @Override
        public View getChildView(int groupPosition, final int childPosition, boolean isLastChild, View convertView, ViewGroup parent) {

            ViewHolder holder;
            if (convertView == null) {
                convertView = inf.inflate(R.layout.detected_modules_row_text, parent, false);
                holder = new ViewHolder();

                holder.text = (TextView) convertView.findViewById(R.id.tvDetectedModuleText);
                convertView.setTag(holder);
            } else {
                holder = (ViewHolder) convertView.getTag();
            }

            holder.text.setText(getChild(groupPosition, childPosition).toString());

            return convertView;
        }
        @Override
        public View getGroupView(int groupPosition, boolean isExpanded, View convertView, ViewGroup parent) {
            ViewHolder holder;

            if (convertView == null) {
                convertView = inf.inflate(R.layout.detected_modules_row, parent, false);

                holder = new ViewHolder();
                holder.text = (TextView) convertView.findViewById(R.id.tvDetectedModule);
                convertView.setTag(holder);
            } else {
                holder = (ViewHolder) convertView.getTag();
            }

            holder.text.setText(getGroup(groupPosition).toString());

            return convertView;
        }
        @Override
        public boolean isChildSelectable(int groupPosition, int childPosition) {
            return true;
        }
        public void updateChild(int groupPosition, ArrayList<String> newChild) {
            children.set(groupPosition,newChild);
        }
        private class ViewHolder {
            TextView text;
        }
    }

    public void addNewModuleToList(Integer id,String text){
        groups.add(text);
        groups_Integer.add(id);
        ArrayList<String> child = new ArrayList<>();
        child.add("----");
        children.add(child);
        listAdapter.notifyDataSetChanged();
    }
    public void updateModuleText(Integer id, String text){
        for (int i=0;i<groups_Integer.size();i++){
            if(groups_Integer.get(i).compareTo(id)==0){
                ArrayList<String> child = new ArrayList<>();
                child.add(text);
                listAdapter.updateChild(i, child);
                listAdapter.notifyDataSetChanged();
                break;
            }
        }
    }
    public void noBleDeviceFound(){
        try {
            TextView textView = (TextView) (V.findViewById(R.id.tvBleDeviceId));
            textView.setText("No bluetooth device present");
        }catch (NullPointerException ignored){}
    }
    public void setBleDeviceIdField(String text){
        try{
            TextView textView = (TextView) (V.findViewById(R.id.tvBleDeviceId));
            textView.setText("Bluetooth module id: "+text);
        }catch (NullPointerException ignored){}
    }
    public void setFrameCounter(String text){
        try {
            TextView textView = (TextView) (V.findViewById(R.id.tvFrameCounter));
            textView.setText(text);
        }catch (NullPointerException ignored){}
    }
    public void setReceiveDataField_set(String text){
        try{
            TextView textView = (TextView) (V.findViewById(R.id.tvReceiveData));
            textView.setText(text);
        }catch (NullPointerException ignored){}
    }
    public void setReceiveDataField_append(String text){
        try {
            TextView textView = (TextView) (V.findViewById(R.id.tvReceiveData));
            textView.append(text);
        }catch (NullPointerException ignored){}
    }
}