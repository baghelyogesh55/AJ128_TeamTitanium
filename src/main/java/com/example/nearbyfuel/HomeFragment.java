package com.example.nearbyfuel;

import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.fragment.app.Fragment;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.Query;
import com.google.firebase.database.ValueEventListener;


public class HomeFragment extends Fragment {

    TextView value;
    TextView quantity;
    TextView mes;
    Button fetch;
    private static final String TAG = "HomeFragment";

    public static float fuel;
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View v= inflater.inflate(R.layout.fragment_home, container, false);
        fetch=v.findViewById(R.id.fetch);
        value=v.findViewById(R.id.display);
        quantity=v.findViewById(R.id.quantity);
        mes=v.findViewById(R.id.message);
        FirebaseDatabase database = FirebaseDatabase.getInstance();

        //Database reference to fetch value from the firebase database.
        DatabaseReference databaseReference = database.getReference();
        final Query lastQuery = databaseReference.child("Weight").orderByKey().limitToLast(1); //database reference pointing to root of database


        fetch.setOnClickListener(new View.OnClickListener() {
            @Override

            public void onClick(View v) {
                lastQuery.addListenerForSingleValueEvent(new ValueEventListener() {

                    String s;

                    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
                    public void onDataChange(@NonNull DataSnapshot dataSnapshot) {
                        String message = dataSnapshot.getValue().toString();
                        Log.d(TAG, "onDataChange: message:"+message);
                        String[] str=message.split(" ");
                        int n=str.length;
                        message=str[0];
                        Log.d(TAG, "onDataChange: message= "+message);
                        int i = message.indexOf('=',8);
                        i++;
                        s = message.substring(i, message.length() - 1);
                        message = s;
                        Log.d(TAG, "onDataChange: message= "+message);
                        float f1 = Float.parseFloat(message);
                        fuel = f1;
                        double q = f1/0.74;
                        quantity.setText(String.format("%.3f",q)+" ml");
                        message=String.format("%.3f",f1);
                        value.setText("Weight  :  " + message+ "gm");
                        if (f1 < 10.00) {
                            mes.setText("\nSeems like you're on low fuel,\nClick Fuel stations ");
                            mes.append(" for nearest Fuel stations");
                        }

                    }

                    public void onCancelled(@NonNull DatabaseError databaseError) {

                        value.setText("Error while fetching the data \nCheck your internet connection or try again later");
                        // Handle possible errors.
                    }
                });
            }

        });
        return v;
    }
}
