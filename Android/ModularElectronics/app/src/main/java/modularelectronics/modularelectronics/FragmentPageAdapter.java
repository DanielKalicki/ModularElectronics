package modularelectronics.modularelectronics;

import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;

import modularelectronics.modularelectronics.DeviceMainFragment.DeviceMainFragment;
import modularelectronics.modularelectronics.ModuleFragment.ModuleFragment;

public class FragmentPageAdapter extends FragmentPagerAdapter {
    int count;

    public FragmentPageAdapter(FragmentManager fm) {
        super(fm);
    }

    @Override
    public Fragment getItem(int arg0) {
        if (arg0==0){
            return new DeviceMainFragment();
        }
        if (arg0>0 && arg0<100){
            return new ModuleFragment();
        }
        return null;
    }

    @Override
    public int getCount() {
        return 100;
    }
    public int getRealCount() {
        return count;
    }
    public void setCount(int countNumb){
        count = countNumb;
    }
}