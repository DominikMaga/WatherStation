using System.Threading.Tasks;
using Measuring.API.Models;

namespace Measuring.API.Data
{
    public interface IMeasureRepository
    {

        Task<Sensor> Register(string physicalShieldId, string physicalSensorId);
        Task<bool> AddMeasure(Sensor sensor, float measuredVale);
    }
}