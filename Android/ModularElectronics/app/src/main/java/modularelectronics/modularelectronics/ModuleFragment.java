package modularelectronics.modularelectronics;

import android.os.Bundle;
import android.support.v4.app.ListFragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;

import java.util.ArrayList;
import java.util.List;

public class ModuleFragment extends ListFragment {
    private List<ListViewItem> mItems;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View V = inflater.inflate(R.layout.module_fragment, container, false);

        mItems = new ArrayList<ListViewItem>();
        mItems.add(new ListViewItem(getResources().getDrawable(R.drawable.ic_launcher), "Value1", "Description1"));
        mItems.add(new ListViewItem(getResources().getDrawable(R.drawable.ic_launcher), "Value2", "Description2"));
        mItems.add(new ListViewItem(getResources().getDrawable(R.drawable.ic_launcher), "Value3", "Description3"));

        ListView list = (ListView) V.findViewById(android.R.id.list);
        setListAdapter(new ListViewModuleInfoAdapter(getActivity(),mItems));
        
        return V;
    }
}

