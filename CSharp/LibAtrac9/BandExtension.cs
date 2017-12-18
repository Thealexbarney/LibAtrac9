using System;

namespace LibAtrac9
{
    internal static class BandExtension
    {
        public static void ApplyBandExtension(Block block)
        {
            if (!block.BandExtensionEnabled || !block.HasExtensionData) return;

            foreach (Channel channel in block.Channels)
            {
                ApplyBandExtensionChannel(channel);
            }
        }

        private static void ApplyBandExtensionChannel(Channel channel)
        {
            int groupAUnit = channel.Block.QuantizationUnitCount;
            int[] scaleFactors = channel.ScaleFactors;
            double[] spectra = channel.Spectra;
            double[] scales = channel.BexScales;
            int[] values = channel.BexValues;

            GetBexBandInfo(out int bandCount, out int groupBUnit, out int groupCUnit, groupAUnit);
            int totalUnits = Math.Max(groupCUnit, 22);

            int groupABin = Tables.QuantUnitToCoeffIndex[groupAUnit];
            int groupBBin = Tables.QuantUnitToCoeffIndex[groupBUnit];
            int groupCBin = Tables.QuantUnitToCoeffIndex[groupCUnit];
            int totalBins = Tables.QuantUnitToCoeffIndex[totalUnits];

            FillHighFrequencies(spectra, groupABin, groupBBin, groupCBin, totalBins);

            switch (channel.BexMode)
            {
                case 0:
                    int bexQuantUnits = totalUnits - groupAUnit;

                    switch (bandCount)
                    {
                        case 3:
                            scales[0] = Tables.BexMode0Bands3[0][values[0]];
                            scales[1] = Tables.BexMode0Bands3[1][values[0]];
                            scales[2] = Tables.BexMode0Bands3[2][values[1]];
                            scales[3] = Tables.BexMode0Bands3[3][values[2]];
                            scales[4] = Tables.BexMode0Bands3[4][values[3]];
                            break;
                        case 4:
                            scales[0] = Tables.BexMode0Bands4[0][values[0]];
                            scales[1] = Tables.BexMode0Bands4[1][values[0]];
                            scales[2] = Tables.BexMode0Bands4[2][values[1]];
                            scales[3] = Tables.BexMode0Bands4[3][values[2]];
                            scales[4] = Tables.BexMode0Bands4[4][values[3]];
                            break;
                        case 5:
                            scales[0] = Tables.BexMode0Bands5[0][values[0]];
                            scales[1] = Tables.BexMode0Bands5[1][values[1]];
                            scales[2] = Tables.BexMode0Bands5[2][values[1]];
                            break;
                    }

                    scales[bexQuantUnits - 1] = Tables.SpectrumScale[scaleFactors[groupAUnit]];

                    AddNoiseToSpectrum(channel, Tables.QuantUnitToCoeffIndex[totalUnits - 1],
                        Tables.QuantUnitToCoeffCount[totalUnits - 1]);
                    ScaleBexQuantUnits(spectra, scales, groupAUnit, totalUnits);
                    break;
                case 1:
                    for (int i = groupAUnit; i < totalUnits; i++)
                    {
                        scales[i - groupAUnit] = Tables.SpectrumScale[scaleFactors[i]];
                    }

                    AddNoiseToSpectrum(channel, groupABin, totalBins - groupABin);
                    ScaleBexQuantUnits(spectra, scales, groupAUnit, totalUnits);
                    break;
                case 2:
                    double groupAScale2 = Tables.BexMode2Scale[values[0]];
                    double groupBScale2 = Tables.BexMode2Scale[values[1]];

                    for (int i = groupABin; i < groupBBin; i++)
                    {
                        spectra[i] *= groupAScale2;
                    }

                    for (int i = groupBBin; i < groupCBin; i++)
                    {
                        spectra[i] *= groupBScale2;
                    }
                    return;
                case 3:
                    double rate = Math.Pow(2, Tables.BexMode3Rate[values[1]]);
                    double scale = Tables.BexMode3Initial[values[0]];
                    for (int i = groupABin; i < totalBins; i++)
                    {
                        scale *= rate;
                        spectra[i] *= scale;
                    }
                    return;
                case 4:
                    double mult = Tables.BexMode4Multiplier[values[0]];
                    double groupAScale4 = 0.7079468 * mult;
                    double groupBScale4 = 0.5011902 * mult;
                    double groupCScale4 = 0.3548279 * mult;

                    for (int i = groupABin; i < groupBBin; i++)
                    {
                        spectra[i] *= groupAScale4;
                    }

                    for (int i = groupBBin; i < groupCBin; i++)
                    {
                        spectra[i] *= groupBScale4;
                    }

                    for (int i = groupCBin; i < totalBins; i++)
                    {
                        spectra[i] *= groupCScale4;
                    }
                    return;
            }
        }

        private static void ScaleBexQuantUnits(double[] spectra, double[] scales, int startUnit, int totalUnits)
        {
            for (int i = startUnit; i < totalUnits; i++)
            {
                for (int k = Tables.QuantUnitToCoeffIndex[i]; k < Tables.QuantUnitToCoeffIndex[i + 1]; k++)
                {
                    spectra[k] *= scales[i - startUnit];
                }
            }
        }

        private static void FillHighFrequencies(double[] spectra, int groupABin, int groupBBin, int groupCBin, int totalBins)
        {
            for (int i = 0; i < groupBBin - groupABin; i++)
            {
                spectra[groupABin + i] = spectra[groupABin - i - 1];
            }

            for (int i = 0; i < groupCBin - groupBBin; i++)
            {
                spectra[groupBBin + i] = spectra[groupBBin - i - 1];
            }

            for (int i = 0; i < totalBins - groupCBin; i++)
            {
                spectra[groupCBin + i] = spectra[groupCBin - i - 1];
            }
        }

        private static void AddNoiseToSpectrum(Channel channel, int index, int count)
        {
            if (channel.Rng == null)
            {
                int[] sf = channel.ScaleFactors;
                ushort seed = (ushort)(543 * (sf[8] + sf[12] + sf[15] + 1));
                channel.Rng = new Atrac9Rng(seed);
            }
            for (int i = 0; i < count; i++)
            {
                channel.Spectra[i + index] = channel.Rng.Next() / 65535.0 * 2.0 - 1.0;
            }
        }

        public static void GetBexBandInfo(out int bandCount, out int groupAUnit, out int groupBUnit, int quantUnits)
        {
            groupAUnit = Tables.BexGroupInfo[quantUnits - 13][0];
            groupBUnit = Tables.BexGroupInfo[quantUnits - 13][1];
            bandCount = Tables.BexGroupInfo[quantUnits - 13][2];
        }
    }
}
