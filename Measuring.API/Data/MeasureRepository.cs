using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Measuring.API.Models;
using Microsoft.EntityFrameworkCore;

namespace Measuring.API.Data
{
    public class MeasureRepository : IMeasureRepository
    {
        private readonly DataContext _context;
        public MeasureRepository(DataContext context)
        {
            _context = context;
        }

        public Sensor Sensor
        {
            get => default(Sensor);
            set
            {
            }
        }

        public async Task<Sensor> Register(string physicalShieldId, string physicalSensorId)
        {
            var shield = await _context.Shields
                .Include(s => s.ShieldConfiguration)
                .FirstOrDefaultAsync(x => x.PhysicalShieldId == physicalShieldId);

            var sensor = await _context.Sensors.FirstOrDefaultAsync(x => x.PhysicalSensorId == physicalSensorId);

            if (shield == null)
            {
                (shield, sensor) = await CreateShieldAndSensor(physicalShieldId, physicalSensorId);
            }
            else if (sensor == null)
            {
                sensor = await CreateSensor(shield, physicalSensorId);
            }

            return sensor;
        }

        public async Task<bool> AddMeasure(Sensor sensor, float measuredVale)
        {
            var measuredValue = new MeasuredValue
            {
                Value = measuredVale,
                Sensor = sensor
            };

            await _context.MeasuredValues.AddAsync(measuredValue);
            await _context.SaveChangesAsync();

            return true;
        }

        private async Task<(Shield, Sensor)> CreateShieldAndSensor(string physicalShieldId, string physicalSensorId)
        {
            var shieldToCreate = new Shield
            {
                PhysicalShieldId = physicalShieldId
            };

            var sensorToCreate = new Sensor
            {
                PhysicalSensorId = physicalSensorId,
                Shield = shieldToCreate
            };

            var shieldConfigurationToCreate = new ShieldConfiguration
            {
                DeepSleep = 0,
                Shield = shieldToCreate
            };

            await _context.Shields.AddAsync(shieldToCreate);
            await _context.Sensors.AddAsync(sensorToCreate);
            await _context.ShieldConfigurations.AddAsync(shieldConfigurationToCreate);
            await _context.SaveChangesAsync();

            return (shieldToCreate, sensorToCreate);
        }

        private async Task<Sensor> CreateSensor(Shield shield, string physicalSensorId)
        {
            var sensorToCreate = new Sensor
            {
                PhysicalSensorId = physicalSensorId,
                Shield = shield
            };

            await _context.Sensors.AddAsync(sensorToCreate);
            await _context.SaveChangesAsync();

            return sensorToCreate;
        }
    }
}