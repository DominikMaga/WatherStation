using System.Threading.Tasks;
using Measuring.API.Data;
using Measuring.API.Dtos;
using Measuring.API.Models;
using Microsoft.AspNetCore.Mvc;

namespace Measuring.API.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class MeasureController : ControllerBase
    {
        private readonly IMeasureRepository _repo;
        public MeasureController(IMeasureRepository repo)
        {
            _repo = repo;
        }

        public DataContext DataContext
        {
            get => default(DataContext);
            set
            {
            }
        }

        [HttpPost("single")]
        public async Task<ShieldConfigurationDto> SingleMeasured(SingleMeasuredDto singleMeasured)
        {
            var sensor = await _repo.Register(singleMeasured.PhysicalShieldId, singleMeasured.PhysicalSensorId);

            await _repo.AddMeasure(sensor, singleMeasured.Value);

            return new ShieldConfigurationDto { DeepSleep = sensor.Shield.ShieldConfiguration.DeepSleep };
        }

        [HttpGet]
        public string TestMessage() => "Message";
    }
}