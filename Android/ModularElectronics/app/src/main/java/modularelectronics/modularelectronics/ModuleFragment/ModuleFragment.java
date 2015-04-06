package modularelectronics.modularelectronics.ModuleFragment;

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
        mItems.add(new ListViewItem(getResources().getDrawable(R.drawable.ic_launcher),varName,varText));
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

