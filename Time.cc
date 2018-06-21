#include <TH1F.h>
#include <TF1.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TMath.h>

#include <fstream>
#include <iostream>
#include <vector>
#include <stdlib.h>
//#include <pair>

void Time(const char *input_name, const char* output_name)
{
    std::ifstream input_file1(input_name);
    int channel;
    long clock_counts;
    int energy;
    int dummy1, dummy2;

    TH1F *hTime = new TH1F("hTime", ";t_{ch0} - t_{ch1}; counts", 41, -20.5, 20.5);

    std::vector<std::pair<long, int>> channel0, channel1;
    // Filling the  two vectors channel0 and channel1 with the time and the energy of the photons that are in the correct energy region
    while (input_file1 >> channel >> clock_counts >> energy >> dummy1 >> dummy2)
    {
        if (energy > 4000 and energy < 4500)
        {
            if (channel == 0)
            {
                channel0.push_back(std::make_pair(clock_counts, energy));
            }
            else if (channel == 1)
            {
                channel1.push_back(std::make_pair(clock_counts, energy));
            }
        }
    }

    uint index_channel1 = 0;
    uint time_window = 20;
    long tmp_min;
    for (auto &item : channel0)
    {
        long minimum = 999999999; /// dummy minimum value
        for (uint index = index_channel1; index < channel1.size(); index++)
        {
            /// updating the minimum value
            tmp_min = item.first - channel1[index].first;
            if (TMath::Abs(tmp_min) <= TMath::Abs(minimum))
            {
                minimum = tmp_min;
                /// For the next element of channel 0 the loop on the second vector will start at index_channel1
                index_channel1 = index;
                continue;
            }
            if (TMath::Abs(minimum) < time_window)
            {
                hTime->Fill(minimum);
            }
            break;
        }
    }

    TCanvas *cTime = new TCanvas("cTime", "cTime");
    cTime->SetLogy();
    hTime->Draw();
    hTime->Fit("gaus");
    TFile f(Form("ROOT_files/%s.root",output_name),"recreate");
    hTime->Write();
    cTime->SaveAs(Form("plots/%s.pdf",output_name));
}