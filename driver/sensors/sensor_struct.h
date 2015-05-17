
struct sensor{
	/* pointer to the init function */
	msg_t (*init_sensor)(void);
	/* pointer to the read function */
	msg_t (*read_sensor)(void);

};

struct sensor_interrupt{
	expchannel_t interrupt_channel;
	uint32_t mode;
	struct sensor sensor;
};

struct sensor_polling{
	uint16_t frequency_hz;
	struct sensor sensor;
};


