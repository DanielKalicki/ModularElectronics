package modularelectronics.modularelectronics.ModuleFragment;

import android.os.Bundle;
import android.support.v4.app.ListFragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;

import java.util.ArrayList;
import java.util.List;

import modularelectronics.modularelectronics.R;

public class ModuleFragment extends ListFragment {
    private List<ListViewItem> mItems;
    ListViewModuleInfoAdapter listAdapter;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View V = inflater.inflate(R.layout.module_fragment, container, false);

        mItems = new ArrayList<>();

        ListView list = (ListView) V.findViewById(android.R.id.list);
        listAdapter = new ListViewModuleInfoAdapter(getActivity(), mItems);
        setListAdapter(listAdapter);
        
        return V;
    }

    public void addVariable(String varName){
        mItems.add(new ListViewItem(getResources().getDrawable(R.drawable.ic_launcher), varName, "---"));
        listAdapter.notifyDataSetChanged();
    }

    public void setVariable(String varName, String valueText){

    }
}

